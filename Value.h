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
            capacity = memory::grow_capacity(old_capacity);
            values   = memory::grow_array(values, old_capacity, capacity);
        }

        values[count] = value;
        ++count;
    }

    void free()
    {
        memory::grow_array(values, capacity * sizeof(Value), 0);
        init();
    }

};