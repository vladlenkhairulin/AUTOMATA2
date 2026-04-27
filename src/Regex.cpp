#include "Regex.h"
#include <iostream>

bool Regex::findPath(const State* s, const std::string& text, size_t pos, size_t& endPos, Match& m, std::map<std::string, size_t>& starts) {

    if (!s->refTag.empty()) {
        if (!m.groups.contains(s->refTag)) return false;
        const std::string& inner = m.groups[s->refTag];
        if (pos + inner.size() > text.size()) return false;
        for (size_t i = 0; i < inner.size(); ++i) {
            if (text[pos + i] != inner[i]) return false;
        }
        size_t newPos = pos + inner.size();
        if (s == nfa.end) {
            endPos = newPos;
            return true;
        }
        if (s->transitions.contains('$')) {
            for (const State* next : s->transitions.at('$')) {
                if (findPath(next, text, newPos, endPos, m, starts))
                    return true;
            }
        }
        return false;
    }
    if (!s->startTag.empty()) {
        starts[s->startTag] = pos;
    }
    if (s == nfa.end) {
        endPos = pos;
        for (const auto& [name, startPos] : starts) {
            if (!m.groups.contains(name)) {
                m.groups[name] = text.substr(startPos, pos - startPos);
            }
        }
        return true;
    }
    bool success = false;
    if (pos < text.size()) {
        if (s->transitions.contains(text[pos])) {
            for (const State* next : s->transitions.at(text[pos])) {
                if (findPath(next, text, pos + 1, endPos, m, starts))
                    return true;
            }
        }
        if (s->transitions.contains('.')) {
            for (const State* next : s->transitions.at('.')) {
                if (findPath(next, text, pos + 1, endPos, m, starts))
                    return true;
            }
        }
    }

    if (s->transitions.contains('$')) {
        for (const State* next : s->transitions.at('$')) {
            if (findPath(next, text, pos, endPos, m, starts))
                success = true;
        }
    }
    if (!s->endTag.empty() && starts.contains(s->endTag) && !m.groups.contains(s->endTag)) {
        m.groups[s->endTag] = text.substr(starts[s->endTag], pos - starts[s->endTag]);
        if (s->transitions.contains('$')) {
            for (const State* next : s->transitions.at('$')) {
                if (findPath(next, text, pos, endPos, m, starts))
                    success = true;
            }
        }
    }
    return success;
}

void Regex::findAll(const std::string& text, std::vector<Match> &output) {
    output.clear();
    if (!nfa.start) return;
    for (size_t i = 0; i < text.size(); i++) {
        Match m;
        size_t endPos = i;
        std::map<std::string, size_t> tempStarts;
        if (findPath(nfa.start, text, i, endPos, m, tempStarts)) {
            std::cout << "Found at " << i << ", length " << (endPos - i) << ", group x = " << m.groups["x"] << std::endl;
            if (endPos > i) {
                m.value = text.substr(i, endPos - i);
                output.push_back(m);
                i = endPos - 1;
            }
        }
    }
}

std::vector<std::string> Regex::findAll(const std::string &text) {
    std::vector<Match> matches;
    findAll(text, matches);
    std::vector<std::string> results;
    for (const Match &m : matches) {
        results.push_back(m.value);
    }
    return results;
}
