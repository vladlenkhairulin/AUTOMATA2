#include "Regex.h"

bool Regex::findPath(State* s, const std::string& text, size_t pos, size_t& endPos, Match& m, std::map<std::string, size_t>& starts) {
    if (!s->startTag.empty()) starts[s->startTag] = pos;
    if (!s->endTag.empty() && starts.contains(s->endTag)) {
        m.groups[s->endTag] = text.substr(starts[s->endTag], pos - starts[s->endTag]);
    }
    if (s == nfa.end) {
        endPos = pos;
        return true;
    }

    if (pos < text.size() && s->transitions.contains(text[pos])) {
        for (const auto next : s->transitions.at(text[pos])){
            if (findPath(next, text, pos + 1, endPos, m, starts)) return true;
        }
    }
    if (pos < text.size() && s->transitions.contains('.')) {
        for (const auto next : s->transitions.at('.')){
            if (findPath(next, text, pos + 1, endPos, m, starts)) return true;
        }
    }
    if (s->transitions.contains('$')) {
        for (const auto next : s->transitions.at('$')) {
            if (findPath(next, text, pos, endPos, m, starts)) return true;
        }
    }
    return false;
}

std::vector<Match> Regex::findAll(const std::string& text) {
    std::vector<Match> results;
    for (size_t i = 0; i < text.size(); i++) {
        Match m;
        size_t endPos = i;
        std::map<std::string, size_t> tempStarts;
        if (findPath(nfa.start, text, i, endPos, m, tempStarts)) {
            m.value = text.substr(i, endPos - i);
            results.push_back(m);
            i = (endPos > i) ? endPos - 1 : i;
        }
    }
    return results;
}