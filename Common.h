#pragma once

#include <stdint.h>
#include <vector>
#include <deque>

#define forever for (;;)

using Byte   = uint8_t;
using Size   = int;
using Index  = int;
using Value  = double; // for now all numbers are doubles!

using Bytes   = std::vector<Byte>;
using Indices = std::vector<Index>;
using Values  = std::vector<Value>;
using ValueStack = std::deque<Value>; // std::stack doesn't allow random access...

/*namespace memory {

static Size grow_capacity(Size old_capacity, Size grow_factor = 2)
{
    // return a multiple of 8 for a new capacity
    return old_capacity < 8 ? 8 : old_capacity * grow_factor;
}

template <class T>
T* grow_array(T* previous, Size old_size, Size new_size)
{
    if (new_size == 0) {
        std::free(previous);
        return nullptr;
    }

    return (T*)std::realloc(previous, new_size);
}
}*/