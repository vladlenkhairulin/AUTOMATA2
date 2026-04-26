#ifndef AUTOMATA2_NFA_H
#define AUTOMATA2_NFA_H

#include <map>
#include <vector>
#include <string>

struct State {
    int id;
    std::map<char, std::vector<State*>> transitions;
    std::string startTag = "";
    std::string endTag = "";
};

struct NFA {
    State* start;
    State* end;
};


#endif //AUTOMATA2_NFA_H