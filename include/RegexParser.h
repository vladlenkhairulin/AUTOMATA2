#ifndef AUTOMATA2_REGEXPARSER_H
#define AUTOMATA2_REGEXPARSER_H

#include <vector>
#include <string>
#include "Tokens.h"

class RegexParser
{
public:
    std::vector<Token> parse(const std::string& regex);

private:
    std::vector<Token> tokenize(const std::string& regex);
    void addConcat(std::vector<Token>& tokens);
    int precedence(TokenType type);
};


#endif //AUTOMATA2_REGEXPARSER_H