#ifndef AUTOMATA2_DFA_H
#define AUTOMATA2_DFA_H

#include "NFA.h"
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <queue>

struct DFAState {
    int id;
    std::set<State*> nfaStates;
    std::map<char, DFAState*> transitions;
    bool isFinal = false;
};

struct DFA {
    DFAState* start;
    std::vector<DFAState*> states;
};


#endif //AUTOMATA2_DFA_H