#include "RegexParser.h"
#include <stack>
#include <cctype>
#include <stdexcept>

std::vector<Token> RegexParser::tokenize(const std::string& regex) {
    std::vector<Token> tokens;
    for (size_t i = 0; i <regex.size(); ++i) {
        char c = regex[i];
        if (c == '&') {
            if (i+1 < regex.size()) {
                char next = regex[i+1];
                if (next == '|' || next=='+' || next=='?' || next=='.' || next=='(' || next==')') {
                    tokens.emplace_back(TokenType::SYMBOL, std::string(1, next));
                    i++;
                    continue;
                }
            }
            tokens.emplace_back(TokenType::SYMBOL, "&");
            continue;
        }
        switch (c) {
            case '|':
                tokens.emplace_back(TokenType::OR);
                break;
            case '+':
                tokens.emplace_back(TokenType::PLUS);
                break;
            case '?':
                tokens.emplace_back(TokenType::OPTION);
                break;
            case '.':
                tokens.emplace_back(TokenType::DOT);
                break;
            case '(':
                tokens.emplace_back(TokenType::LPAR);
                break;
            case ')':
                tokens.emplace_back(TokenType::RPAR);
                break;
            case '{':
            {
                std::string num1, num2;
                i++;
                while (i < regex.size() && std::isdigit(regex[i])) {
                    num1.push_back(regex[i]);
                    i++;
                }
                if (i >= regex.size() || regex[i] != ',') {
                    throw std::runtime_error("Invalid repeat: missing comma");
                }
                i++;
                while (i < regex.size() && std::isdigit(regex[i])) {
                    num2.push_back(regex[i]);
                    i++;
                }
                if (i >= regex.size() || regex[i] != '}') {
                    throw std::runtime_error("Invalid repeat: missing '}'");
                }
                i++;

                Token t(TokenType::REPEAT);
                t.value = num1 + "," + num2;
                tokens.push_back(t);
                continue;
            }
            default:
                if (std::isprint(static_cast<unsigned char>(c))) {
                    tokens.emplace_back(TokenType::SYMBOL, std::string(1, c));
                }
                break;
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
            if ((t1 == TokenType::SYMBOL || t1 == TokenType::RPAR || t1 == TokenType::DOT || t1==TokenType::PLUS || t1==TokenType::OPTION || t1 == TokenType::REPEAT)
                && (t2==TokenType::SYMBOL || t2==TokenType::LPAR || t2 == TokenType::DOT || t2 == TokenType::REPEAT)) needConcat = true;
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
    if (type == TokenType::REPEAT) return 4;
    return 0;
}

std::vector<Token> RegexParser::parse(const std::string& regex) {
    auto tokens = tokenize(regex);
    addConcat(tokens);
    std::vector<Token> res;
    std::stack<Token> operations;

    for (const auto& token: tokens) {
        if (token.type == TokenType::SYMBOL || token.type == TokenType::DOT || token.type == TokenType::REPEAT) res.push_back(token);
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