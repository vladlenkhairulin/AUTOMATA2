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
public:
    NFA build(const std::string& regex);
};


#endif //AUTOMATA2_THOMPSON_H