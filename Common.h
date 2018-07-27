#pragma once

#define forever for (;;)

#ifdef _DEBUG
#define WHEN_DEBUG(code) code
#else
#define WHEN_DEBUG(code)
#endif

using Byte   = uint8_t;
using Size   = int;
using Index  = int;
using Value  = double; // for now all numbers are doubles!

namespace memory {

// those functions might prove useless and a std::vector for the dynamic arrays would be a lot easier...
// if there is no deeper learning effect in sight, I might just delete them...

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
}