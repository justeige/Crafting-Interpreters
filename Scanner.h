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
            return make_token(Token::Eof);
        }

        char c = advance();

        // handle numbers and identifier first
        if (std::isalpha(c)) { return make_identifier(); }
        if (std::isdigit(c)) { return make_number(); }

        switch (c) {
        // one-char lexeme
        case '(':
            return make_token(Token::LeftParen);
        case ')':
            return make_token(Token::RightParen);
        case '{':
            return make_token(Token::LeftBrace);
        case '}':
            return make_token(Token::RightBrace);
        case ';':
            return make_token(Token::Semicolon);
        case ',':
            return make_token(Token::Comma);
        case '.':
            return make_token(Token::Dot);
        case '-':
            return make_token(Token::Minus);
        case '+':
            return make_token(Token::Plus);
        case '/':
            return make_token(Token::Slash);
        case '*':
            return make_token(Token::Star);

        // two-char lexeme
        case '!':
            return make_token(match('=') ? Token::BangEqual    : Token::Bang);
        case '=':
            return make_token(match('=') ? Token::EqualEqual   : Token::Equal);
        case '<':
            return make_token(match('=') ? Token::LessEqual    : Token::Less);
        case '>':
            return make_token(match('=') ? Token::GreaterEqual : Token::Greater);

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

    Token make_token(Token::Type type)
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
        token.type = Token::Error;
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
        return make_token(Token::String);
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

        return make_token(Token::Number);
    }

    Token make_identifier()
    {
        while (std::isalpha(peek()) || std::isdigit(peek())) {
            advance();
        }

        return make_token(identifier_type());
    }

    Token::Type identifier_type()
    {
        switch (start[0]) {
        case 'a':
            return check_keyword(1, 2, "nd", Token::And);
        case 'c':
            return check_keyword(1, 4, "lass", Token::Class);
        case 'e':
            return check_keyword(1, 3, "lse", Token::Else);
        case 'f':
            if (current - start > 1) {
                switch (start[1]) {
                case 'a':
                    return check_keyword(2, 3, "lse", Token::False);
                case 'o':
                    return check_keyword(2, 1, "r", Token::For);
                case 'u':
                    return check_keyword(2, 1, "n", Token::Fun);
                }
            }
            break;
        case 'i':
            return check_keyword(1, 1, "f", Token::If);
        case 'n':
            return check_keyword(1, 2, "il", Token::Nil);
        case 'o':
            return check_keyword(1, 1, "r", Token::Or);
        case 'p':
            return check_keyword(1, 4, "rint", Token::Print);
        case 'r':
            return check_keyword(1, 5, "eturn", Token::Return);
        case 's':
            return check_keyword(1, 4, "uper", Token::Super);
        case 't':
            if (current - start > 1) {
                switch (start[1]) {
                case 'h':
                    return check_keyword(2, 2, "is", Token::This);
                case 'r':
                    return check_keyword(2, 2, "ue", Token::True);
                }
            }
            break;
        case 'v':
            return check_keyword(1, 2, "ar", Token::Var);
        case 'w':
            return check_keyword(1, 4, "hile", Token::While);
        }

        return Token::Identifier;
    }

    Token::Type check_keyword(int begin, int length, const char* rest, Token::Type type)
    {
        if ((int)(current - start) == begin + length && std::memcmp(start + begin, rest, length) == 0) {
            return type;
        }

        return Token::Identifier;
    }
};
