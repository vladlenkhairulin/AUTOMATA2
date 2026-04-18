#include "DFAversion.h"

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