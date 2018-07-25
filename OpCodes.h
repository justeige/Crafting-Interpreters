#pragma once

/// TODO make enum class?
enum OpCode {
    OP_Constant, //  two bytes: [OpCode][Constant Index]
    OP_Return    // one byte: [OpCode]
};