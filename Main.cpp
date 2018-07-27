#include <iostream>
#include <array>

#include "Common.h"
#include "Chunk.h"
#include "Debug.h"
#include "VM.h"

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
    vm.interpret(c);

    return 0;
}

