#ifndef AUTOMATA2_REGEX_H
#define AUTOMATA2_REGEX_H
#include "Thompson.h"
#include "Match.h"
#include "NFA.h"
#include "DFAversion.h"

class Regex {
private:
    NFA nfa;
    struct PathStruct {
        const std::string* text;
        size_t pos;
        size_t endPos;
        Match match;
        std::map<std::string, size_t> starts;
    };
    bool findPath(const State* s, PathStruct& args);

    bool tryRefTag(const State* s, PathStruct& args);
    bool tryCharTransitions(const State* s, PathStruct& args);
    bool tryDotTransitions(const State* s, PathStruct& args);
    bool tryEpsilonTransitions(const State* s, PathStruct& args);
    bool tryEndTag(const State* s, PathStruct& args);
    void saveStartedGroups(PathStruct& args);

public:
    void compile (const std::string& regex) {
        Thompson thompson;
        nfa = thompson.build(regex);
    }
    Regex(const std::string& regex) {
        compile(regex);
    }

    static std::vector<std::string> findAll(const std::string& regex, const std::string& text);
    static void findAll(const std::string& regex, const std::string& text, std::vector<Match>& output);
};
#endif //AUTOMATA2_REGEX_H