#include "StateElimination.h"

#include <memory>

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
    std::vector<std::unique_ptr<GNFAState>> gnfaStates;
    for (DFAState* ds: dfa.states) {
        auto gs = std::make_unique<GNFAState>();
        gs->id = ds->id;
        transformMap[ds] = gs.get();
        gnfaStates.push_back(std::move(gs));
    }
    for (DFAState* ds: dfa.states) {
        GNFAState* gs = transformMap[ds];
        for (const auto& t: ds->transitions) {
            std::string label(1, t.first);
            if (t.first == '.') label = ".";
            gs->trans[transformMap[t.second]] = label;
        }
    }

    auto newStart = std::make_unique<GNFAState>();
    newStart->id = -1;
    auto newEnd = std::make_unique<GNFAState>();
    newEnd->id = -2;

    GNFAState* start = newStart.get();
    GNFAState* end = newEnd.get();
    gnfaStates.push_back(std::move(newStart));
    gnfaStates.push_back(std::move(newEnd));

    start->trans[transformMap[dfa.start]] = "$";
    for (DFAState* ds: dfa.states) {
        if (ds->isFinal) {
            transformMap[ds]->trans[end] = "$";
        }
    }
    std::vector<GNFAState*> toRemove;
    for (auto& state: gnfaStates) {
        if (state.get() != start && state.get() != end) {
            toRemove.push_back(state.get());
        }
    }

    for (GNFAState* state: toRemove) {
        std::string loop = state->trans[state];
        if (loop.empty()) loop = "$";
        std::vector<std::pair<GNFAState*, std::string>> ins;
        std::vector<std::pair<GNFAState*, std::string>> outs;
        for (auto& s: gnfaStates) {
            if (s.get() == state) continue;
            if (s->trans.contains(state)) {
                ins.emplace_back(s.get(), s->trans[state]);
            }
        }

        for (auto& tr: state->trans) {
            if (tr.first != state) {
                outs.emplace_back(tr.first, tr.second);
            }
        }
        for (auto& inPair : ins) {
            for (auto& outPair : outs) {
                std::string bypass = opConcat(inPair.second, opConcat(opStar(loop), outPair.second));
                GNFAState* inS = inPair.first;
                GNFAState* outS = outPair.first;
                if (inS->trans.contains(outS)) {
                    inS->trans[outS] = opOr(inS->trans[outS], bypass);
                }
                else {
                    inS->trans[outS] = bypass;
                }
            }
        }
        for (auto& s: gnfaStates) {
            s->trans.erase(state);
        }
    }
    return start->trans.contains(end) ? start->trans[end] : "";
}