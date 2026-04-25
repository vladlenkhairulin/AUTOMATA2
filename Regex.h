#ifndef AUTOMATA2_REGEX_H
#define AUTOMATA2_REGEX_H
#include "Thompson.h"
#include "Match.h"
#include "NFA.h"
#include "DFAVersion.h"

class Regex {
private:
    NFA nfa;
    bool findPath(const State* s, const std::string& text, size_t pos, size_t& endPos, Match& m, std::map<std::string, size_t>& starts);
public:
    void compile (const std::string& regex) {
        Thompson thompson;
        nfa = thompson.build(regex);
    }
    Regex(const std::string& regex) {
        compile(regex);
    }
    std::vector<std::string> findAll(const std::string& text);
    void findAll(const std::string& text, std::vector<Match>& output);
};
#endif //AUTOMATA2_REGEX_H