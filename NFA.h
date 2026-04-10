#ifndef AUTOMATA2_NFA_H
#define AUTOMATA2_NFA_H

#include <map>
#include <vector>

struct State {
    int id;
    std::map<char, std::vector<State*>> transitions;
};

struct NFA {
    State* start;
    State* end;
};


#endif //AUTOMATA2_NFA_H