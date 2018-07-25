#pragma once

#include "Common.h"
#include "Chunk.h"

namespace Debug {


static void   show(Chunk& chunk, const char* name);
static Index show(Chunk& chunk, Index current);
static Index simple_instruction(const char* name, Index offset);
static Index constant_instruction(const char* name, Chunk& chunk, Index offset);

// print every operation in a chunk
static void show(Chunk& chunk, const char* name)
{
    std::printf("%s \n", name);
    for (int i = 0; i < chunk.count;/**/) {
        i = show(chunk, i);
    }
}

// print an instruction at a specific Index
static Index show(Chunk& chunk, Index Index)
{
    std::printf("%04d ", Index);
    OpCode instruction = (OpCode)chunk.code[Index];
    switch (instruction) {
    case OP_Constant:
        return constant_instruction("OP_CONSTANT", chunk, Index);
    case OP_Return:
        return simple_instruction("RETURN", Index);
    default:
        std::printf("%d unkown opcode!\n", instruction);
        break;
    }
    return Index + 1;
}

// print a simple instruction (returns, breaks, ...)
static Index simple_instruction(const char* name, Index offset)
{
    std::printf("%s\n", name);
    return offset + 1;
}

static Index constant_instruction(const char* name, Chunk& chunk, Index offset)
{
    Byte constant_index = chunk.code[offset + 1];
    std::printf("%-16s %4d '", name, constant_index);
    std::printf("%g", chunk.constants.values[constant_index]);
    std::printf("'\n");
    return offset + 2; // one for the opcode, one for the index of the value!
}

}