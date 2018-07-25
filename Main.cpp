#include <iostream>

#include "Common.h"
#include "Value.h"
#include "Chunk.h"
#include "Debug.h"

int main(int argc, const char** argv)
{
    Chunk c;

    // write a constant (2 bytes!)
    Index constant = c.add_const(1.2);
    c.write(OP_Constant);
    c.write(constant);

    c.write(OP_Return);
    Debug::show(c, "Test");
    c.free();
    return 0;
}

