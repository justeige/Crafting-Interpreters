#pragma once

#include "Common.h"

// all constant values are part of the "Constant Pool" which is a value array (similar to the byte-code array)

struct ValueArray {
    Value* values = nullptr;
    Size  count    = 0; // elements in the array
    Size  capacity = 0; // number of allocated elements

    ValueArray() = default;

    void init()
    {
        values   = nullptr;
        count    = 0;
        capacity = 0;
    }

    void write(Value value)
    {
        // needs new allocation?
        if (capacity < count + 1) {
            auto old_capacity = capacity;
            capacity = grow_capacity(old_capacity);
            values = grow_array(values, old_capacity, capacity);
        }

        values[count] = value;
        ++count;
    }

    void free()
    {
        grow_array(values, capacity * sizeof(Value), 0);
        init();
    }

    // return a multiple of 8 for a new capacity
    Size grow_capacity(Size old_capacity, Size grow_factor = 2) const
    {
        return old_capacity < 8 ? 8 : old_capacity * grow_factor;
    }

    Value* grow_array(Value* previous, Size old_size, Size new_size)
    {
        if (new_size == 0) {
            std::free(previous);
            return nullptr;
        }

        return (Value*) std::realloc(previous, new_size);
    }
};