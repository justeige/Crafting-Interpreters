#pragma once

#include "Common.h"
#include "Chunk.h"
#include "OpCodes.h"

namespace Debug {


static void  show(Chunk& chunk, const char* name);
static Index show(Chunk& chunk, Index current);
static Index simple_instruction(const char* name, Index offset);
static Index constant_instruction(const char* name, Chunk& chunk, Index offset);

// print every operation in a chunk
static void show(Chunk& chunk, const char* name)
{
    std::printf("%s \n", name);
    std::printf("=================================\n");
    std::printf("Index|Line| OpCode        |Values\n");
    std::printf("=================================\n");
    for (int i = 0; i < chunk.code.size();/**/) {
        i = show(chunk, i);
    }
}

// print an instruction at a specific Index
static Index show(Chunk& chunk, Index offset)
{
    std::printf("%04d ", offset);

    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        std::printf("     | ");
    }
    else {
        std::printf("%4d | ", chunk.lines[offset]);
    }

    OpCode instruction = (OpCode)chunk.code[offset];
    switch (instruction) {
    case OP_Constant:
        return constant_instruction("CONSTANT", chunk, offset);
    case OP_Negate:
        return simple_instruction("NEGATE", offset);
    case OP_Add:
        return simple_instruction("ADD", offset);
    case OP_Subtract:
        return simple_instruction("SUBTRACT", offset);
    case OP_Multiply:
        return simple_instruction("MULTIPLY", offset);
    case OP_Divide:
        return simple_instruction("Divide", offset);
    case OP_Return:
        return simple_instruction("RETURN", offset);
    default:
        std::printf("%d unkown opcode!\n", instruction);
        break;
    }
    return offset + 1;
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
    std::printf("%g", chunk.constants[constant_index]);
    std::printf("'\n");
    return offset + 2; // one for the opcode, one for the index of the value!
}

}