#pragma once

#include "Common.h"
#include "Chunk.h"
#include "Scanner.h"
#include "OpCodes.h"
#include "Token.h"

enum class InterpretResult {
    Ok,
    CompileError,
    RuntimeError
};

struct Parser {
    Token current;
    Token previous;
    bool error_raised = false;
    bool panic_raised = false; // used instead of exceptions to unwind the whole stack

    Parser() = default;
};

/// make enum class ?
enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! - +
    PREC_CALL,        // . () []
    PREC_PRIMARY
};


// forward decl. for function ptr
struct VM;

typedef void (VM::*ParseFn)(); /// change to either 'using' or std::function ...

struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};
using ParseRules = std::vector<ParseRule>;


// virtual machine
struct VM {

    /// function pointer to member functions will be tricky... not sure if I can have the same design as the book
    /// implements here...
    ParseRules rules = {
        { &VM::grouping, NULL,    PREC_CALL },       // TOKEN_LEFT_PAREN
        { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN
        { NULL,     NULL,    PREC_NONE },       // TOKEN_LEFT_BRACE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA
        { NULL,     NULL,    PREC_CALL },       // TOKEN_DOT
        { &VM::unary, &VM::binary,  PREC_TERM },       // TOKEN_MINUS
        { NULL,     &VM::binary,  PREC_TERM },       // TOKEN_PLUS
        { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON
        { NULL,     &VM::binary,  PREC_FACTOR },     // TOKEN_SLASH
        { NULL,     &VM::binary,  PREC_FACTOR },     // TOKEN_STAR
        { NULL,     NULL,    PREC_NONE },       // TOKEN_BANG
        { NULL,     NULL,    PREC_EQUALITY },   // TOKEN_BANG_EQUAL
        { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL
        { NULL,     NULL,    PREC_EQUALITY },   // TOKEN_EQUAL_EQUAL
        { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_GREATER
        { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_GREATER_EQUAL
        { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_LESS
        { NULL,     NULL,    PREC_COMPARISON }, // TOKEN_LESS_EQUAL
        { NULL,     NULL,    PREC_NONE },       // TOKEN_IDENTIFIER
        { NULL,     NULL,    PREC_NONE },       // TOKEN_STRING
        { &VM::number,   NULL,    PREC_NONE },       // TOKEN_NUMBER
        { NULL,     NULL,    PREC_AND },        // TOKEN_AND
        { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS
        { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_FALSE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN
        { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR
        { NULL,     NULL,    PREC_NONE },       // TOKEN_IF
        { NULL,     NULL,    PREC_NONE },       // TOKEN_NIL
        { NULL,     NULL,    PREC_OR },         // TOKEN_OR
        { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT
        { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN
        { NULL,     NULL,    PREC_NONE },       // TOKEN_SUPER
        { NULL,     NULL,    PREC_NONE },       // TOKEN_THIS
        { NULL,     NULL,    PREC_NONE },       // TOKEN_TRUE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR
        { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE
        { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR
        { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF
    };


    Chunk chunk;
    std::size_t ip = 0; // instruction pointer
    ValueStack stack;
    std::unique_ptr<Scanner> scanner;

    // part of compiler!
    Parser parser;

    VM() = default;

    Byte read_byte()
    {
        return chunk.code[ip++];
    }

    Value read_const()
    {
        return chunk.constants[read_byte()];
    }

    void push(Value value)
    {
        stack.push_back(value);
    }

    Value pop()
    {
        Value top = stack.back();
        stack.pop_back();
        return top;
    }

    InterpretResult interpret(Chunk c)
    {
        chunk = c;
        ip = 0;
        return run();
    }

    InterpretResult interpret(std::string const& src)
    {
        Chunk new_chunk = {};
        if (!compile(src, new_chunk)) {
            return InterpretResult::CompileError;
        }

        chunk = new_chunk;
        ip = 0;

        auto result = run();
        assert(result == InterpretResult::Ok);

        return result;
    }

    void print_stack() const
    {
        for (std::size_t n = 0; n < stack.size(); ++n) {
            auto slot = stack[n];
            std::printf("[%g]", slot);
        }
        std::printf("\n");
    }

    InterpretResult run()
    {
        // just to make the code a little more readable:
        using IR = InterpretResult;

        forever {

            print_stack();

            auto instruction = (OpCode)read_byte();
            switch (instruction) {

            case OP_Constant: {
                Value constant = read_const();
                push(constant);
                break;
            }

            case OP_Add: {
                Value a = pop();
                Value b = pop();
                push(a + b);
                break;
            }

            case OP_Subtract: {
                Value a = pop();
                Value b = pop();
                push(a - b);
                break;
            }

            case OP_Multiply: {
                Value a = pop();
                Value b = pop();
                push(a * b);
                break;
            }

            case OP_Divide: {
                Value a = pop();
                Value b = pop();
                push(a / b);
                break;
            }

            case OP_Negate: {
                push(-pop());
                break;
            }

            case OP_Return: {
                std::printf("return %g\n", pop());
                return IR::Ok;
            }

            default:
                break;
            }
        }

        return InterpretResult::Ok;
    }

    bool compile(std::string const& src, Chunk& chunk)
    {
        scanner = std::make_unique<Scanner>(src.c_str());
        compiling_chunk = &chunk; // move in compiler constructor

        advance();
        expression();
        consume(TOKEN_EOF, "Expected EoF token!");
        end_compiler();

        return !parser.error_raised;
    }

    // should be part of compiler object!
    void advance()
    {
        parser.previous = parser.current;

        forever {
            parser.current = scanner->scan_token();
            if (parser.current.type != TOKEN_ERROR) { break; }

            error_at_current(parser.current.start);
        }
    }

    void error_at_current(const char* msg)
    {
        error_at(&parser.current, msg);
    }

    void error(const char* msg)
    {
        error_at(&parser.previous, msg);
    }

    void error_at(Token* token, const char* msg)
    {
        if (parser.panic_raised) { return; }
        parser.panic_raised = true;

        std::fprintf(stderr, "[line %d] Error", token->line);

        if (token->type == TOKEN_EOF) {
            std::fprintf(stderr, " at end");
        }
        else if (token->type == TOKEN_ERROR) {
            // Nothing.
        }
        else {
            std::fprintf(stderr, " at '%.*s'", token->length, token->start);
        }

        std::fprintf(stderr, ": %s\n", msg);
        parser.error_raised = true;
    }

    void consume(TokenType type, const char* msg)
    {
        if (parser.current.type == type) {
            advance();
            return;
        }

        error_at_current(msg);
    }

    void expression()
    {
        parse_precedence(PREC_ASSIGNMENT);
    }

    void number()
    {
        double value = std::strtod(parser.previous.start, NULL);
        emit_constant(value);
    }

    void emit_byte(Byte byte)
    {
        auto current = current_chunk();
        current->write(byte, parser.previous.line);
    }

    void emit_constant(Value value)
    {
        emit_bytes(OP_Constant, make_constant(value));
    }

    Byte make_constant(Value value)
    {
        auto c = current_chunk();
        auto constant = c->add_const(value);
        if (constant > UINT8_MAX) {
            error("Too many constants in one chunk.");
            return 0;
        }

        return (Byte) constant;
    }

    void grouping()
    {
        expression();
        consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
    }

    void unary()
    {
        TokenType operator_type = parser.previous.type;

        // compile operand
        expression();

        // emit operator instruction
        switch (operator_type) {
        case TOKEN_MINUS:
            emit_byte(OP_Negate);
            break;
        default:
            assert(false);
            break;
        }
    }

    void binary()
    {
        // remember operator
        TokenType operatorType = parser.previous.type;

        // compile right operand.
        ParseRule* rule = get_rule(operatorType);
        parse_precedence((Precedence)(rule->precedence + 1));

        // Emit the operator instruction.
        switch (operatorType) {
        case TOKEN_PLUS:
            emit_byte(OP_Add);
            break;
        case TOKEN_MINUS:
            emit_byte(OP_Subtract);
            break;
        case TOKEN_STAR:
            emit_byte(OP_Multiply);
            break;
        case TOKEN_SLASH:
            emit_byte(OP_Divide);
            break;
        default:
            return; // Unreachable.
        }
    }

    ParseRule* get_rule(TokenType type)
    {
        return &rules[type];
    }

    void parse_precedence(Precedence prec)
    {
        advance();
        auto prefixRule = get_rule(parser.previous.type);
        auto prefix = prefixRule->prefix;
        if (prefixRule == NULL) {
            error("Expect expression.");
            return;
        }

        (*this.*prefix)(); // pretty sure thats the most horrible expression that I have ever to write in c++...

        while (prec <= get_rule(parser.current.type)->precedence) {
            advance();
            auto infixRule = get_rule(parser.previous.type);
            auto infix = infixRule->infix;
            (*this.*infix)();
        }
    }

    void emit_bytes(Byte byte1, Byte byte2)
    {
        emit_byte(byte1);
        emit_byte(byte2);
    }

    Chunk* compiling_chunk = nullptr;

    Chunk* current_chunk()
    {
        return compiling_chunk;
    }

    void end_compiler()
    {
        emit_return();
    }

    void emit_return()
    {
        emit_byte(OP_Return);
    }
};