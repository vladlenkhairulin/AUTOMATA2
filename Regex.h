#ifndef AUTOMATA2_REGEX_H
#define AUTOMATA2_REGEX_H
#include "Thompson.h"
#include "Match.h"
#include "NFA.h"
#include "DFAVersion.h"

class Regex {
private:
    NFA nfa;
    Thompson thompson;
    bool findPath(State* s, const std::string& text, size_t pos, size_t& endPos, Match& m, std::map<std::string, size_t>& starts);
public:
    void compile (const std::string& regex) {
        nfa = thompson.build(regex);
    }
    std::vector<Match> findAll(const std::string& text);
};


#endif //AUTOMATA2_REGEX_H