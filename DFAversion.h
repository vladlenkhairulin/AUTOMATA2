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

class DFAversion {
public:
    std::set<State*> epsilonClosure(const std::set<State*>& nfaStates);
    std::set<State*> move(const std::set<State*>& states, char symbol);
    std::set<char> getAlphabet(NFA& nfa);
    DFA convert(NFA nfa);
};
#endif //AUTOMATA2_DFA_H