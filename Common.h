#pragma once

#include <stdint.h>
#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <cassert>

#define forever for (;;)

using Byte   = uint8_t;
using Size   = int;
using Index  = int;

using Bytes   = std::vector<Byte>;
using Indices = std::vector<Index>;