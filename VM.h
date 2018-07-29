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

// virtual machine
struct VM {

    Chunk chunks;
    std::size_t ip = 0; // instruction pointer
    ValueStack stack;
    std::unique_ptr<Scanner> scanner;

    VM() = default;

    Byte read_byte()
    {
        return chunks.code[ip++];
    }

    Value read_const()
    {
        return chunks.constants[read_byte()];
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
        chunks = c;
        ip = 0;
        return run();
    }

    InterpretResult interpret(std::string const& src)
    {
        compile(src);
        return InterpretResult::Ok;
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

    void compile(std::string const& src)
    {
        scanner = std::make_unique<Scanner>(src.c_str());

        int line = -1;
        forever {
            Token token = scanner->scan_token();
            if (token.line != line) {
                std::printf("%4d ", token.line);
                line = token.line;
            }
            else {
                std::printf("   | ");
            }
            std::printf("%2d '%.*s'\n", token.type, token.length, token.start);

            if (token.type == TOKEN_EOF) { break; }


        }
    }
};