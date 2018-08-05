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

enum class Precedence {
    None,
    Assignment,  // =
    Or,          // or
    And,         // and
    Equality,    // == !=
    Comparison,  // < > <= >=
    Term,        // + -
    Factor,      // * /
    Unary,       // ! - +
    Call,        // . () []
    Primary
};



// virtual machine
struct VM {

    using Prec = Precedence; // just for a little less typing
    typedef void (VM::*ParseFn)(); /// change to either 'using' or std::function ...

    struct ParseRule {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
    };
    using ParseRules = std::vector<ParseRule>;

    ParseRules rules = {
        { &VM::grouping,    nullptr,         Prec::Call },       // TOKEN_LEFT_PAREN
        { nullptr,          nullptr,         Prec::None },       // TOKEN_RIGHT_PAREN
        { nullptr,          nullptr,         Prec::None },       // TOKEN_LEFT_BRACE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_RIGHT_BRACE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_COMMA
        { nullptr,          nullptr,         Prec::Call },       // TOKEN_DOT
        { &VM::unary,       &VM::binary,     Prec::Term },       // TOKEN_MINUS
        { nullptr,          &VM::binary,     Prec::Term },       // TOKEN_PLUS
        { nullptr,          nullptr,         Prec::None },       // TOKEN_SEMICOLON
        { nullptr,          &VM::binary,     Prec::Factor },     // TOKEN_SLASH
        { nullptr,          &VM::binary,     Prec::Factor },     // TOKEN_STAR
        { nullptr,          nullptr,         Prec::None },       // TOKEN_BANG
        { nullptr,          nullptr,         Prec::Equality },   // TOKEN_BANG_EQUAL
        { nullptr,          nullptr,         Prec::None },       // TOKEN_EQUAL
        { nullptr,          nullptr,         Prec::Equality },   // TOKEN_EQUAL_EQUAL
        { nullptr,          nullptr,         Prec::Comparison }, // TOKEN_GREATER
        { nullptr,          nullptr,         Prec::Comparison }, // TOKEN_GREATER_EQUAL
        { nullptr,          nullptr,         Prec::Comparison }, // TOKEN_LESS
        { nullptr,          nullptr,         Prec::Comparison }, // TOKEN_LESS_EQUAL
        { nullptr,          nullptr,         Prec::None },       // TOKEN_IDENTIFIER
        { nullptr,          nullptr,         Prec::None },       // TOKEN_STRING
        { &VM::number,      nullptr,         Prec::None },       // TOKEN_NUMBER
        { nullptr,          nullptr,         Prec::And },        // TOKEN_AND
        { nullptr,          nullptr,         Prec::None },       // TOKEN_CLASS
        { nullptr,          nullptr,         Prec::None },       // TOKEN_ELSE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_FALSE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_FUN
        { nullptr,          nullptr,         Prec::None },       // TOKEN_FOR
        { nullptr,          nullptr,         Prec::None },       // TOKEN_IF
        { nullptr,          nullptr,         Prec::None },       // TOKEN_NIL
        { nullptr,          nullptr,         Prec::Or },         // TOKEN_OR
        { nullptr,          nullptr,         Prec::None },       // TOKEN_PRINT
        { nullptr,          nullptr,         Prec::None },       // TOKEN_RETURN
        { nullptr,          nullptr,         Prec::None },       // TOKEN_SUPER
        { nullptr,          nullptr,         Prec::None },       // TOKEN_THIS
        { nullptr,          nullptr,         Prec::None },       // TOKEN_TRUE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_VAR
        { nullptr,          nullptr,         Prec::None },       // TOKEN_WHILE
        { nullptr,          nullptr,         Prec::None },       // TOKEN_ERROR
        { nullptr,          nullptr,         Prec::None },       // TOKEN_EOF
    };


    Chunk chunk;
    std::size_t ip = 0; // instruction pointer
    ValueStack stack;
    std::unique_ptr<Scanner> scanner;

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
        consume(Token::Eof, "Expected EoF token!");
        end_compiler();

        return !parser.error_raised;
    }

    // should be part of compiler object!
    void advance()
    {
        parser.previous = parser.current;

        forever {
            parser.current = scanner->scan_token();
            if (parser.current.type != Token::Error) { break; }

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

        if (token->type == Token::Eof) {
            std::fprintf(stderr, " at end");
        }
        else if (token->type == Token::Error) {
            // Nothing.
        }
        else {
            std::fprintf(stderr, " at '%.*s'", token->length, token->start);
        }

        std::fprintf(stderr, ": %s\n", msg);
        parser.error_raised = true;
    }

    void consume(Token::Type type, const char* msg)
    {
        if (parser.current.type == type) {
            advance();
            return;
        }

        error_at_current(msg);
    }

    void expression()
    {
        parse_precedence(Prec::Assignment);
    }

    void number()
    {
        double value = std::strtod(parser.previous.start, NULL);
        emit_constant(value);
    }

    void emit_byte(Byte byte)
    {
        auto current = compiling_chunk;
        current->write(byte, parser.previous.line);
    }

    void emit_constant(Value value)
    {
        emit_bytes(OP_Constant, make_constant(value));
    }

    Byte make_constant(Value value)
    {
        auto c = compiling_chunk;
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
        consume(Token::RightParen, "Expected ')' after expression.");
    }

    void unary()
    {
        Token::Type operator_type = parser.previous.type;

        // compile operand
        expression();

        // emit operator instruction
        switch (operator_type) {
        case Token::Minus:
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
        Token::Type operatorType = parser.previous.type;

        // compile right operand.
        ParseRule* rule = get_rule(operatorType);
        auto next_prec_level = (int)rule->precedence + 1;
        parse_precedence((Precedence)(next_prec_level));

        // Emit the operator instruction.
        switch (operatorType) {
        case Token::Plus:
            emit_byte(OP_Add);
            break;
        case Token::Minus:
            emit_byte(OP_Subtract);
            break;
        case Token::Star:
            emit_byte(OP_Multiply);
            break;
        case Token::Slash:
            emit_byte(OP_Divide);
            break;
        default:
            return; // Unreachable.
        }
    }

    ParseRule* get_rule(Token::Type type)
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

    void end_compiler()
    {
        emit_return();
    }

    void emit_return()
    {
        emit_byte(OP_Return);
    }
};