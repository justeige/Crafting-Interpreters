#pragma once

struct Token {
    // Embedding the type enum in the Token struct makes for more readable code.
    // Using an enum class would create pretty verbose expressions...
    enum Type {
        // Single-character tokens.
        LeftParen, RightParen,
        LeftBrace, RightBrace,
        Comma, Dot, Minus, Plus,
        Semicolon, Slash, Star,

        // One or two character tokens.
        Bang, BangEqual,
        Equal, EqualEqual,
        Greater, GreaterEqual,
        Less, LessEqual,

        // Literals.
        Identifier, String, Number,

        // Keywords.
        And, Class, Else, False,
        Fun, For, If, Nil, Or,
        Print, Return, Super, This,
        True, Var, While,

        Error,
        Eof
    };

    Type type;
    const char* start;
    int length;
    int line;
};