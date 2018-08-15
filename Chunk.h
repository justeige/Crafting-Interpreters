#pragma once


#include "Common.h"
#include "Value.h"

// chunk := simple dynamic array
struct Chunk {

    Bytes code; // byte code
    Indices lines; // line information
    Values constants;

    Chunk() = default;

    void write(Byte byte, Index line)
    {
        // append the new byte to the list
        code.push_back(byte);
        lines.push_back(line);
    }

    void clear()
    {
        code.clear();
        lines.clear();
        constants.clear();
    }

    Index add_const(Value value)
    {
        constants.push_back(value);
        return constants.size() - 1;
    }
};


using Chunks = std::vector<Chunk>;