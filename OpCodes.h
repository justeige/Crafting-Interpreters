#pragma once

#include "Common.h"

enum OpCode : Byte {
    OP_Constant = 1, //  two bytes: [OpCode][Constant Index]

    // binary operations
    OP_Add,
    OP_Subtract,
    OP_Multiply,
    OP_Divide,

    // unary operations
    OP_Negate,
    OP_Return
};

using OpCodes = std::vector<OpCode>;