#ifndef AUTOMATA2_TOKENS_H
#define AUTOMATA2_TOKENS_H

#include <string>

enum class TokenType
{
    SYMBOL,
    OR,
    CONCAT,
    PLUS,
    OPTION,
    DOT,
    LPAR,
    RPAR,
    REPEAT
};

struct Token
{
    TokenType type;
    std::string value;
    Token(TokenType t, const std::string& v = "") : type(t), value(v) {}
};

#endif //AUTOMATA2_TOKENS_H