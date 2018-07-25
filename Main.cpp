#include <iostream>

#include "Common.h"
#include "Chunk.h"
#include "Debug.h"

int main(int argc, const char** argv)
{
    Chunk c;
    c.write(OP_Return);
    Debug::show(c, "Test");
    c.free();
    return 0;
}

