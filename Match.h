#ifndef AUTOMATA2_MATCH_H
#define AUTOMATA2_MATCH_H

#include <string>
#include <map>

struct Match {
    std::string value;
    std::map<std::string, std::string> groups;
    std::string operator[](const std::string& name) {
        return groups.contains(name) ? groups[name] : "";
    }
};

#endif //AUTOMATA2_MATCH_H