#ifndef AUTOMATA2_SYNTAXTREE_H
#define AUTOMATA2_SYNTAXTREE_H


#include <vector>
#include "Tokens.h"

struct Node {
    Token token;
    Node* left;
    Node* right;
    Node(const Token& t) : token(t), left(nullptr), right(nullptr) {}
};

class SyntaxTree {
public:
    Node* build(const std::vector<Token>& postfix);
};


#endif //AUTOMATA2_SYNTAXTREE_H