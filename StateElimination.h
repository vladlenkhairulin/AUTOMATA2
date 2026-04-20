#ifndef AUTOMATA2_STATEELIMINATION_H
#define AUTOMATA2_STATEELIMINATION_H
#include "DFAversion.h"
#include <string>
#include <vector>
#include <set>
#include <map>


class StateElimination {
private:
    struct GNFAState {
        int id;
        std::map<GNFAState*, std::string> trans;
    };
    std::string opOr(const std::string& a, const std::string& b);
    std::string opConcat(const std::string& a, const std::string& b);
    std::string opStar(const std::string& a);
public:
    std::string toRegex(const DFA& dfa);
};


#endif //AUTOMATA2_STATEELIMINATION_H