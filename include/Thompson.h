#ifndef AUTOMATA2_THOMPSON_H
#define AUTOMATA2_THOMPSON_H
#include "SyntaxTree.h"
#include "NFA.h"
#include "RegexParser.h"

class Thompson {
private:
    NFA buildRec(Node* node);
    State* newState();
    int counter = 0;
    NFA buildStar(Node* node);
public:
    NFA build(const std::string& regex);
    Node* cloneAst(Node* node);
};


#endif //AUTOMATA2_THOMPSON_H