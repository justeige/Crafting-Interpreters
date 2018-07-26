#pragma once
#include <stdint.h>
#include <cstdlib> // for std::realloc

#include "Common.h"
#include "Value.h"

// chunk := simple dynamic array
struct Chunk {

    Byte* code   = nullptr; // byte code
    Index* lines = nullptr; // line information
    Size  count = 0;      // elements in the array
    Size  capacity = 0;   // number of allocated elements
    ValueArray constants = {};

    Chunk() = default;

    void init()
    {
        code     = nullptr;
        count    = 0;
        capacity = 0;
    }

    void write(Byte byte, Index line)
    {
        // needs new allocation?
        if (capacity < count + 1) {
            auto old_capacity = capacity;
            capacity = memory::grow_capacity(old_capacity);
            code     = memory::grow_array(code, old_capacity, capacity);
            lines    = memory::grow_array(lines, old_capacity, capacity);
        }

        // append the new byte to the list
        code[count]  = byte;
        lines[count] = line;
        ++count;
    }

    void free()
    {
        memory::grow_array(code, capacity * sizeof(Byte), 0);
        constants.free();
        init();
    }

    Index add_const(Value value)
    {
        constants.write(value);
        return constants.count - 1;
    }
};


