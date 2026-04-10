#include "RegexParser.h"
#include <stack>
#include <cctype>

std::vector<Token> RegexParser::tokenize(const std::string& regex) {
    std::vector<Token> tokens;
    for (char c : regex) {
        if (std::isalnum(c)) {
            tokens.emplace_back(TokenType::SYMBOL, std::string(1, c));
        }
        else if (c == '|') {
            tokens.emplace_back(TokenType::OR);
        }
        else if (c == '+') {
            tokens.emplace_back(TokenType::PLUS);
        }
        else if (c == '?') {
            tokens.emplace_back(TokenType::OPTION);
        }
        else if (c == '.') {
            tokens.emplace_back(TokenType::DOT);
        }
        else if (c == '(') {
            tokens.emplace_back(TokenType::LPAR);
        }
        else if (c == ')') {
            tokens.emplace_back(TokenType::RPAR);
        }
    }
    return tokens;
}

void RegexParser::addConcat(std::vector<Token>& tokens) {
    std::vector<Token> res;
    for (size_t i = 0; i < tokens.size(); ++i) {
        res.push_back(tokens[i]);
        if (i+1 < tokens.size()) {
            TokenType t1 = tokens[i].type;
            TokenType t2 = tokens[i+1].type;
            bool needConcat = false;
            if ((t1 == TokenType::SYMBOL || t1 == TokenType::RPAR || t1==TokenType::PLUS || t1==TokenType::OPTION)
                && (t2==TokenType::SYMBOL || t2==TokenType::LPAR)) needConcat = true;
            if (needConcat) {
                res.emplace_back(TokenType::CONCAT);
            }

        }
    }
    tokens = res;
}

int RegexParser::precedence(TokenType type) {
    if (type == TokenType::OR) return 1;
    if (type == TokenType::CONCAT) return 2;
    if (type == TokenType::PLUS || type == TokenType::OPTION) return 3;
    return 0;
}

std::vector<Token> RegexParser::parse(const std::string& regex) {
    auto tokens = tokenize(regex);
    addConcat(tokens);
    std::vector<Token> res;
    std::stack<Token> operations;

    for (const auto& token: tokens) {
        if (token.type == TokenType::SYMBOL) res.push_back(token);
        else if (token.type == TokenType::LPAR) operations.push(token);
        else if (token.type == TokenType::RPAR) {
            while (!operations.empty() && operations.top().type != TokenType::LPAR) {
                res.push_back(operations.top());
                operations.pop();
            }
            if (!operations.empty()) operations.pop();
        }
        else {
            while (!operations.empty() && precedence(operations.top().type) >= precedence(token.type)) {
                res.push_back(operations.top());
                operations.pop();
            }
            operations.push(token);
        }
    }
    while (!operations.empty()) {
        res.push_back(operations.top());
        operations.pop();
    }
    return res;
}