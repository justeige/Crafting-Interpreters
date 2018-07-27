#include <iostream>
#include <array>

#include "Common.h"
#include "Value.h"
#include "Chunk.h"
#include "Debug.h"

enum class InterpretResult {
    Ok,
    CompileError,
    RuntimeError
};

// virtual machine
struct VM {

    static const std::size_t STACK_MAX = 1024;

    Chunk* chunks    = nullptr;
    Byte*  ip        = nullptr; // instruction pointer
    std::size_t top = 0;

    std::array<Value, STACK_MAX> stack;

    VM() = default;

    Byte read_byte()
    {
        return *ip++;
    }

    Value read_const()
    {
        return chunks->constants.values[read_byte()];
    }

    void reset_stack()
    {
        top = 0;
    }

    void push(Value value)
    {
        stack[top] = value;
        top++;
    }

    Value pop()
    {
        top--;
        return stack[top];
    }

    InterpretResult interpret(Chunk* c)
    {
        chunks = c;
        ip = chunks->code;
        return run();
    }

    void print_stack() const
    {
        for (std::size_t n = 0; n < top; ++n) {
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
};

int main(int argc, const char** argv)
{
    VM vm;

    Chunk c;

    const Index line = 99;

    // write a constant (2 bytes!)
    Index constant = c.add_const(1.2);
    c.write(OP_Constant, line);
    c.write(constant, line);

    constant = c.add_const(3.7);
    c.write(OP_Constant, line);
    c.write(constant, line);

    c.write(OP_Add, line);

    // write the return
    c.write(OP_Return, line);
    Debug::show(c, "Test");

    std::printf("\nStart interpreting:\n");
    vm.interpret(&c);

    return 0;
}

