#include "DFAversion.h"
#include "Thompson.h"

std::set<State*> DFAversion::epsilonClosure(const std::set<State*>& nfaStates) {
    std::set<State*> closure = nfaStates;
    std::stack<State*> st;
    for (State* s: nfaStates) {
        st.push(s);
    }
    while (!st.empty()) {
        State* cur = st.top();
        st.pop();
        for (auto next: cur->transitions) {
            if (next.first == '$') {
                for (auto target: next.second) {
                    if (!closure.count(target)) {
                        closure.insert(target);
                        st.push(target);
                    }
                }
            }
        }
    }
    return closure;
}

std::set<State*> DFAversion::move(const std::set<State*>& states, char symbol) {
    std::set<State*> res;
    for (State* s: states) {
        if (s->transitions.count(symbol)) {
            for (auto target: s->transitions.at(symbol)) {
                res.insert(target);
            }
        }
        if (s->transitions.count('.')) {
            for (auto target : s->transitions.at('.')) {
                res.insert(target);
            }
        }
    }
    return res;
}

std::set<char> DFAversion::getAlphabet(NFA& nfa) {
    std::set<char> alphabet;
    std::stack<State*> st;
    std::set<State*> visited;
    st.push(nfa.start);

    while (!st.empty()) {
        State* s = st.top();
        st.pop();
        if (visited.count(s)) continue;
        visited.insert(s);
        for (auto& next : s->transitions) {
            if (next.first != '$' && next.first != '.') alphabet.insert(next.first);
            for (auto t : next.second) st.push(t);
        }
    }
    return alphabet;
}

DFA DFAversion::convert(NFA nfa) {
    DFA dfa;
    int idCounter = 0;
    std::map<std::set<State*>, DFAState*> map;
    std::queue<std::set<State*>> q;

    std::set<State*> startSet;
    startSet.insert(nfa.start);
    std::set<State*> startClosure = epsilonClosure(startSet);

    DFAState* startState = new DFAState();
    startState->id = idCounter++;
    startState->nfaStates = startClosure;

    dfa.start = startState;
    dfa.states.push_back(startState);
    map[startClosure] = startState;
    q.push(startClosure);
    std::set<char> alphabet = getAlphabet(nfa);

    while (!q.empty()) {
        std::set<State*> curSet = q.front();
        q.pop();
        DFAState* curDFA = map[curSet];

        for (char c : alphabet) {
            std::set<State*> moved = move(curSet, c);
            if (moved.empty()) continue;
            std::set<State*> closure = epsilonClosure(moved);
            if (!map.count(closure)) {
                DFAState* newState = new DFAState{};
                newState->id = idCounter++;
                newState->nfaStates = closure;

                map[closure] = newState;
                dfa.states.push_back(newState);
                q.push(closure);
            }
            curDFA->transitions[c] = map[closure];
        }
    }
    for (DFAState* dfaState : dfa.states) {
        for (State* nfaState: dfaState->nfaStates ) {
            if (nfaState == nfa.end) {
                dfaState->isFinal = true;
                break;
            }
        }
    }
    return dfa;
}

std::set<char> DFAversion::getAlphabetDFA(const DFA& dfa) {
    std::set<char> alphabet;
    for (DFAState* state: dfa.states) {
        for (const auto& trans: state->transitions) {
            alphabet.insert(trans.first);
        }
    }
    return alphabet;
}

DFA DFAversion::minimize(const DFA& oldDFA) {
    if (oldDFA.states.empty()) return {};
    std::set<char> alphabet = getAlphabetDFA(oldDFA);
    std::vector<std::vector<DFAState*>> partitions;
    std::vector<DFAState*> finals, nonFinals;
    for (DFAState* s: oldDFA.states) {
        if (s->isFinal) finals.push_back(s);
        else nonFinals.push_back(s);

    }
    if (!finals.empty()) partitions.push_back(finals);
    if (!nonFinals.empty()) partitions.push_back(nonFinals);
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<std::vector<DFAState*>> newPartitions;
        for (auto& group : partitions) {
            if (group.size()<=1) {
                newPartitions.push_back(group);
                continue;
            }
            std::map<DFAState*, int> groupID;
            for (int gid = 0; gid < static_cast<int>(partitions.size()); ++gid) {
                for (DFAState* st : partitions[gid]) {
                    groupID[st] = gid;
                }
            }
            std::map<std::map<char, int>, std::vector<DFAState*>> subGroups;
            for (DFAState* s: group) {
                std::map<char, int> sig;
                for (char c: alphabet) {
                    auto it = s->transitions.find(c);
                    if (it != s->transitions.end() && it->second != nullptr) {
                        auto gidIt = groupID.find(it->second);
                        if (gidIt != groupID.end()) sig[c] = gidIt->second;
                        else sig[c] = -1;
                    }
                    else sig[c] = -1;
                }
                subGroups[sig].push_back(s);
            }
            for (auto& sub : subGroups) {
                newPartitions.push_back(sub.second);
            }
            if (subGroups.size() > 1) changed = true;
        }
        partitions = std::move(newPartitions);
    }

    DFA minDFA;
    int newID = 0;
    std::map<DFAState*, DFAState*> overhaul;
    for (auto& group : partitions) {
        DFAState* newS = new DFAState();
        newS->id = newID++;
        newS->isFinal = false;
        for (DFAState* s : group) {
            if (s->isFinal) {
                newS->isFinal = true;
                break;
            }
        }
        minDFA.states.push_back(newS);
        for (DFAState* oldS : group) {
            overhaul[oldS] = newS;
        }
    }

    auto it = overhaul.find(oldDFA.start);
    if (it != overhaul.end()) {
        minDFA.start = it->second;
    }

    for (auto& group : partitions) {
        if (group.empty()) continue;
        DFAState* src = overhaul[group[0]];
        for (char c : alphabet) {
            auto it = group[0]->transitions.find(c);
            if (it != group[0]->transitions.end() && it->second) {
                auto targetIt = overhaul.find(it->second);
                if (targetIt != overhaul.end()) {
                    src->transitions[c] = targetIt->second;
                }
            }
        }
    }
    return minDFA;
}

DFA DFAversion::compile(const std::string& regex) {
    Thompson thompson;
    const NFA nfa = thompson.build(regex);
    const DFA rawDFA = convert(nfa);
    DFA minDFA = minimize(rawDFA);
    return minDFA;
}