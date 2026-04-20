#include "StateElimination.h"

std::string StateElimination::opOr(const std::string &a, const std::string &b) {
    if (a.empty() || a =="$") return b;
    if (b.empty() || b == "$") return a;
    if (a==b) return a;
    return "(" + a + "|" + b + ")";
}

std::string StateElimination::opConcat(const std::string &a, const std::string &b) {
    if (a.empty() || a == "$") return b;
    if (b.empty() || b == "$") return a;
    return a+b;
}


std::string StateElimination::opStar(const std::string &a) {
    if (a.empty() || a=="$") return "";
    std::string option;
    if (a.length() == 1) {
        option = a + "?";
    }
    else {
        option = "(" + a + ")?";
    }

    if (option.length() == 2) return option + "+";
    return "(" + option + ")+";
}

std::string StateElimination::toRegex(const DFA& dfa) {
    if (dfa.states.empty()) return "";
    std::map<DFAState*, GNFAState*> transformMap;
    std::vector<GNFAState*> gnfaStates;
    for (DFAState* ds: dfa.states) {
        GNFAState* gs = new GNFAState();
        gs->id = ds->id;
        transformMap[ds] = gs;
        gnfaStates.push_back(gs);
    }
    for (DFAState* ds: dfa.states) {
        GNFAState* gs = transformMap[ds];
        for (const auto& t: ds->transitions) {
            std::string label(1, t.first);
            if (t.first == '.') label = ".";
            gs->trans[transformMap[t.second]] = label;
        }
    }

    GNFAState* newStart = new GNFAState();
    newStart->id = -1;
    GNFAState* newEnd = new GNFAState();
    newEnd->id = -2;
    gnfaStates.push_back(newStart);
    gnfaStates.push_back(newEnd);
    newStart->trans[transformMap[dfa.start]] = "$";
    for (DFAState* ds: dfa.states) {
        if (ds->isFinal) {
            transformMap[ds]->trans[newEnd] = "$";
        }
    }
    std::vector<GNFAState*> toRemove;
    for (GNFAState* state: gnfaStates) {
        if (state != newStart && state != newEnd) {
            toRemove.push_back(state);
        }
    }

    for (GNFAState* state: toRemove) {
        std::string loop = state->trans[state];
        if (loop.empty()) loop = "$";
        std::vector<std::pair<GNFAState*, std::string>> ins;
        std::vector<std::pair<GNFAState*, std::string>> outs;
        for (GNFAState* s: gnfaStates) {
            if (s == state) continue;
            auto it = s->trans.find(state);
            if (it != s->trans.end()) {
                ins.emplace_back(s, it->second);
            }
        }

        for (auto& tr: state->trans) {
            if (tr.first != state) {
                outs.emplace_back(tr.first, tr.second);
            }
        }
        for (auto& inPair: ins) {
            GNFAState* inState = inPair.first;
            std::string regex1 = inPair.second;
            for (auto& outPair: outs) {
                GNFAState* outState = outPair.first;
                std::string regex2 = outPair.second;
                std::string newLabel = opConcat(regex1, opConcat(opStar(loop), regex2));
                auto it = inState->trans.find(outState);
                if (it != inState->trans.end()) {
                    inState->trans[outState] = opOr(it->second, newLabel);
                }
                else inState->trans[outState] = newLabel;
            }
        }
        for (GNFAState* s: gnfaStates) {
            s->trans.erase(state);
        }
    }
    auto it = newStart->trans.find(newEnd);
    std::string res;
    if (it != newStart->trans.end()) res = it->second;
    else res = "";
    for (GNFAState* s: gnfaStates) delete s;
    return res;
}