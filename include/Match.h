#ifndef AUTOMATA2_MATCH_H
#define AUTOMATA2_MATCH_H

#include <string>
#include <map>

struct Match {
    std::string value;
    std::map<std::string, std::string> groups;
    std::string& operator[](const std::string& name) {
        return groups[name];
    }
    const std::string& operator[](const std::string& name) const {
        static const std::string empty = "";
        auto it = groups.find(name);
        if (it == groups.end()) {
            return empty;
        }
        return it->second;
    }

    std::string operator[](size_t index) {
        if (index == 0) return value;
        if (index > groups.size()) return "";
        auto it = groups.begin();
        std::advance(it, index-1);
        return it->second;
    }

    std::string operator[](size_t index) const {
        if (index == 0) return value;
        if (index > groups.size()) return "";
        auto it = groups.begin();
        std::advance(it, index - 1);
        return it->second;
    }

    auto begin() {return groups.begin();}
    auto end() {return groups.end();}
};

#endif //AUTOMATA2_MATCH_H