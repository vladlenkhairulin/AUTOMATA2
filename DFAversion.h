#ifndef AUTOMATA2_DFA_H
#define AUTOMATA2_DFA_H

#include "NFA.h"
#include <map>
#include <set>
#include <vector>
#include <string>
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
private:
    std::set<char> getAlphabet(const NFA& nfa);
    std::set<char> getAlphabetDFA(const DFA& dfa);
public:
    std::set<State*> epsilonClosure(const std::set<State*>& nfaStates);
    std::set<State*> move(const std::set<State*>& states, char symbol);
    DFA convert(const NFA& nfa);
    DFA minimize(const DFA& dfa);
    DFA compile(const std::string& regex);
    DFA complement(const DFA& oldDFA);
    DFA reverse(const DFA& oldDFA);
};
#endif //AUTOMATA2_DFA_H