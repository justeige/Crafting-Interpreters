#pragma once
#include <stdint.h>
#include <cstdlib> // for std::realloc

#include "Common.h"
#include "Value.h"

// chunk := simple dynamic array
struct Chunk {

    Byte* code = nullptr; // byte code
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

    void write(Byte byte)
    {
        // needs new allocation?
        if (capacity < count + 1) {
            auto old_capacity = capacity;
            capacity = grow_capacity(old_capacity);
            code = grow_array(code, old_capacity, capacity);
        }

        // append the new byte to the list
        code[count] = byte;
        ++count;
    }

    void free()
    {
        grow_array(code, capacity * sizeof(Byte), 0);
        constants.free();
        init();
    }

    // return a multiple of 8 for a new capacity
    Size grow_capacity(Size old_capacity, Size grow_factor = 2) const
    {
        return old_capacity < 8 ? 8 : old_capacity * grow_factor;
    }

    Byte* grow_array(Byte* previous, Size old_size, Size new_size)
    {
        if (new_size == 0) {
            std::free(previous);
            return nullptr;
        }

        return (Byte*) std::realloc(previous, new_size);
    }

    Index add_const(Value value)
    {
        constants.write(value);
        return constants.count - 1;
    }
};


