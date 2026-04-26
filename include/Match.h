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

    std::string operator[](size_t index) const {
        if (index == 0) return value;
        if (index > groups.size()) return "";
        auto it = groups.begin();
        std::advance(it, index-1);
        return it->second;
    }

    auto begin() {return groups.begin();}
    auto end() {return groups.end();}
};

#endif //AUTOMATA2_MATCH_H