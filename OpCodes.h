#pragma once

/// TODO make enum class?
enum OpCode {
    OP_Constant, //  two bytes: [OpCode][Constant Index]

    // binary operations
    OP_Add,
    OP_Subtract,
    OP_Multiply,
    OP_Divide,

    // unary operations
    OP_Negate,
    OP_Return
};