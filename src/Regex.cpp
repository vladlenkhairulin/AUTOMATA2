#include "Regex.h"

void Regex::saveStartedGroups(PathStruct& args) {
    const std::string& text = *args.text;
    for (const auto& [grpName, startPos] : args.starts) {
        if (!args.match.groups.contains(grpName)) {
            args.match[grpName] = text.substr(startPos, args.pos - startPos);
        }
    }
}

bool Regex::tryRefTag(const State* s, Regex::PathStruct& args) {
    PathStruct ctxt = args;
    saveStartedGroups(ctxt);
    if (!args.match.groups.contains(s->refTag)) return false;
    const std::string& text = *ctxt.text;
    const std::string& captured = ctxt.match[s->refTag];
    if (ctxt.pos + captured.size() > text.size()) return false;
    for (size_t i = 0; i<captured.size(); ++i) {
        if (text[ctxt.pos+i] != captured[i]) return false;
    }
    ctxt.pos += captured.size();
    if (s == nfa.end) {
        ctxt.endPos = ctxt.pos;
        args = ctxt;
        return true;
    }
    if (!s->transitions.contains('$')) return false;
    for (const State* next : s->transitions.at('$')) {
        PathStruct nextArgs = ctxt;
        if (findPath(next, nextArgs)) {
            args = nextArgs;
            return true;
        }
    }
    return false;
}

bool Regex::tryCharTransitions(const State* s, PathStruct& args) {
    const std::string& text = *args.text;
    if (args.pos >= text.size()) return false;
    char cur = text[args.pos];
    if (!s->transitions.contains(cur)) return false;
    for (const State* next : s->transitions.at(cur)) {
        PathStruct nextArgs = args;
        nextArgs.pos++;
        if (findPath(next, nextArgs)) {
            args = nextArgs;
            return true;
        }
    }
    return false;
}

bool Regex::tryDotTransitions(const State* s, PathStruct& args) {
const std::string& text = *args.text;
    if (args.pos >= text.size()) return false;
    if (!s->transitions.contains('.')) return false;
    for (const State* next : s->transitions.at('.')) {
        PathStruct nextArgs = args;
        nextArgs.pos++;
        if (findPath(next, nextArgs)){
            args = nextArgs;
            return true;
        }
    }

    return false;
}

bool Regex::tryEpsilonTransitions(const State* s, PathStruct& args) {
    if (!s->transitions.contains('$')) return false;
    for (const State* next : s->transitions.at('$')) {
        PathStruct nextArgs = args;
        if (findPath(next, nextArgs)) {
            args = nextArgs;
            return true;
        }
    }
    return false;
}

bool Regex::tryEndTag(const State* s, PathStruct& args) {
    if (s->endTag.empty()) return false;
    if (!args.starts.contains(s->endTag)) return false;
    if (!args.match.groups.contains(s->endTag)) {
        size_t startPos = args.starts[s->endTag];
        args.match[s->endTag] = args.text->substr(startPos, args.pos - startPos);
    }
    return tryEpsilonTransitions(s, args);
}

bool Regex::findPath(const State* s, PathStruct& args) {
    if (!s) return false;
    if (!s->refTag.empty()) return tryRefTag(s, args);
    if (!s->startTag.empty()) {
        args.starts[s->startTag] = args.pos;
    }
    if (!s->endTag.empty() && args.starts.contains(s->endTag)) {
        const std::string& text = *args.text;
        size_t startPos = args.starts[s->endTag];
        args.match[s->endTag] = text.substr(startPos, args.pos-startPos);
    }
    if (s == nfa.end){
        args.endPos = args.pos;
        saveStartedGroups(args);
        return true;
    }

    if (tryCharTransitions(s, args)) return true;
    if (tryDotTransitions(s, args)) return true;
    if (tryEpsilonTransitions(s, args)) return true;
    return false;
}

void Regex::findAll(const std::string& regex, const std::string& text, std::vector<Match>& output) {
    output.clear();
    Regex re(regex);
    if (!re.nfa.start) return;
    for (size_t i = 0; i <= text.size(); ++i) {
        PathStruct ctxt;
        ctxt.text = &text;
        ctxt.pos = i;
        ctxt.endPos = i;
        if (!re.findPath(re.nfa.start, ctxt)) continue;
        if (ctxt.endPos < i) continue;
        if (ctxt.endPos == i && !text.empty()) continue;
        ctxt.match.value = text.substr(i, ctxt.endPos-i);
        output.push_back(ctxt.match);
        if (ctxt.endPos == i) break;
        i = ctxt.endPos - 1;
    }
}

std::vector<std::string> Regex::findAll(const std::string& regex, const std::string &text) {
    std::vector<Match> matches;
    findAll(regex, text, matches);
    std::vector<std::string> results;
    for (const Match &m : matches) {
        results.push_back(m.value);
    }
    return results;
}
