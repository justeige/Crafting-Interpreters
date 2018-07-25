#pragma once

#include "Common.h"
#include "Chunk.h"

namespace Debug {


static void   show(Chunk& chunk, const char* name);
static Offset show(Chunk& chunk, Offset current);
static Offset simple_instruction(const char* name, Offset current);


// print every operation in a chunk
static void show(Chunk& chunk, const char* name)
{
    std::printf("%s \n", name);
    for (int i = 0; i < chunk.count;/**/) {
        i = show(chunk, i);
    }
}

// print an instruction at a specific offset
static Offset show(Chunk& chunk, Offset current)
{
    std::printf("%04d ", current);
    OpCode instruction = (OpCode)chunk.code[current];
    switch (instruction) {
    case OP_Return:
        return simple_instruction("RETURN", current);
    default:
        std::printf("%d unkown opcode!", instruction);
        break;
    }
    return current + 1;
}

// print a simple instruction (returns, breaks, ...)
static Offset simple_instruction(const char* name, Offset current)
{
    std::printf("%s\n", name);
    return current + 1;
}

}