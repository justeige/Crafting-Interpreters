#pragma once

#include <cctype> // for std::isdigit
#include "Token.h"

struct Scanner {
    const char* start;
    const char* current;
    int line = 1;

    Scanner(const char* src) : start(src), current(src) {}

    Token scan_token()
    {
        skip_whitespace();

        start = current;

        if (eof()) {
            return make_token(TOKEN_EOF);
        }

        char c = advance();

        // handle numbers and identifier first
        if (std::isalpha(c)) { return make_identifier(); }
        if (std::isdigit(c)) { return make_number(); }

        switch (c) {
        // one-char lexeme
        case '(':
            return make_token(TOKEN_LEFT_PAREN);
        case ')':
            return make_token(TOKEN_RIGHT_PAREN);
        case '{':
            return make_token(TOKEN_LEFT_BRACE);
        case '}':
            return make_token(TOKEN_RIGHT_BRACE);
        case ';':
            return make_token(TOKEN_SEMICOLON);
        case ',':
            return make_token(TOKEN_COMMA);
        case '.':
            return make_token(TOKEN_DOT);
        case '-':
            return make_token(TOKEN_MINUS);
        case '+':
            return make_token(TOKEN_PLUS);
        case '/':
            return make_token(TOKEN_SLASH);
        case '*':
            return make_token(TOKEN_STAR);

        // two-char lexeme
        case '!':
            return make_token(match('=') ? TOKEN_BANG_EQUAL    : TOKEN_BANG);
        case '=':
            return make_token(match('=') ? TOKEN_EQUAL_EQUAL   : TOKEN_EQUAL);
        case '<':
            return make_token(match('=') ? TOKEN_LESS_EQUAL    : TOKEN_LESS);
        case '>':
            return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

        // string handling
        case '"':
            return make_string();

        default:
            break;
        }

        return error_token("Unexpected character!");
    }

    bool eof() const
    {
        return *current == '\0';
    }

    Token make_token(TokenType type)
    {
        Token token;
        token.type = type;
        token.start = start;
        token.length = (int)(current - start);
        token.line = line;

        return token;
    }

    Token error_token(const char* msg)
    {
        Token token;
        token.type = TOKEN_ERROR;
        token.start = msg;
        token.length = (int)std::strlen(msg);
        token.line = line;

        return token;
    }

    char advance()
    {
        current++;
        return current[-1];
    }

    bool match(char expected)
    {
        if (eof()) { return false; }
        if (*current != expected) { return false; }

        current++;
        return true;
    }

    void skip_whitespace()
    {
        forever {
            char c = peek();
            switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                line++;
                advance();
                break;

            case '/':
                if (peek_next() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !eof()) { advance(); }
                }
                else {
                    return;
                }
                break;

            default:
                return;
            }
        }
    }

    char peek()
    {
        return *current;
    }

    char peek_next()
    {
        if (eof()) { return '\0'; }
        return current[1];
    }

    Token make_string()
    {
        while (peek() != '"' && !eof()) {
            if (peek() == '\n') { line++; }
            advance();
        }

        if (eof()) { return error_token("Unterminated string."); }

        // the closing '"'
        advance();
        return make_token(TOKEN_STRING);
    }

    Token make_number()
    {
        while (std::isdigit(peek())) { advance(); }

        // Look for a fractional part.
        if (peek() == '.' && std::isdigit(peek_next())) {
            // Consume the "."
            advance();

            while (std::isdigit(peek())) { advance(); }
        }

        return make_token(TOKEN_NUMBER);
    }

    Token make_identifier()
    {
        while (std::isalpha(peek()) || std::isdigit(peek())) {
            advance();
        }

        return make_token(identifier_type());
    }

    TokenType identifier_type()
    {
        switch (start[0]) {
        case 'a':
            return check_keyword(1, 2, "nd", TOKEN_AND);
        case 'c':
            return check_keyword(1, 4, "lass", TOKEN_CLASS);
        case 'e':
            return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (current - start > 1) {
                switch (start[1]) {
                case 'a':
                    return check_keyword(2, 3, "lse", TOKEN_FALSE);
                case 'o':
                    return check_keyword(2, 1, "r", TOKEN_FOR);
                case 'u':
                    return check_keyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i':
            return check_keyword(1, 1, "f", TOKEN_IF);
        case 'n':
            return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'o':
            return check_keyword(1, 1, "r", TOKEN_OR);
        case 'p':
            return check_keyword(1, 4, "rint", TOKEN_PRINT);
        case 'r':
            return check_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 's':
            return check_keyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (current - start > 1) {
                switch (start[1]) {
                case 'h':
                    return check_keyword(2, 2, "is", TOKEN_THIS);
                case 'r':
                    return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v':
            return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'w':
            return check_keyword(1, 4, "hile", TOKEN_WHILE);
        }

        return TOKEN_IDENTIFIER;
    }

    TokenType check_keyword(int begin, int length, const char* rest, TokenType type)
    {
        if ((int)(current - start) == begin + length && std::memcmp(start + begin, rest, length) == 0) {
            return type;
        }

        return TOKEN_IDENTIFIER;
    }
};
