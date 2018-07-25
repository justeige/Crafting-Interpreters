#pragma once

using Byte   = uint8_t;
using Size   = int;
using Offset = int;
using Value  = double; // for now all numbers are doubles!

enum OpCode {
    OP_Constant,
    OP_Return
};