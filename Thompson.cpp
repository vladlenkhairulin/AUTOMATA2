#include "Thompson.h"

State* Thompson::newState() {
    State* state = new State();
    state->id = counter++;
    return state;
}

NFA Thompson::build(const std::string& regex) {
    RegexParser parser;
    SyntaxTree tree;
    auto postfix = parser.parse(regex);
    Node* root = tree.build(postfix);
    return buildRec(root);
}

NFA Thompson::buildRec(Node* node) {
    if (node->token.type == TokenType::SYMBOL) {
        State* s1 = newState();
        State* s2 = newState();
        s1->transitions[node->token.value[0]].push_back(s2);
        return {s1, s2};
    }

    if (node->token.type == TokenType::DOT) {
        State* s1 = newState();
        State* s2 = newState();
        s1->transitions['.'].push_back(s2);
        return {s1, s2};
    }

    if (node->token.type == TokenType::OR) {
        NFA l = buildRec(node->left);
        NFA r = buildRec(node->right);
        State* newStart = newState();
        State* newEnd = newState();
        newStart->transitions['$'].push_back(l.start);
        newStart->transitions['$'].push_back(r.start);
        l.end->transitions['$'].push_back(newEnd);
        r.end->transitions['$'].push_back(newEnd);
        return {newStart, newEnd};
    }
    if (node->token.type == TokenType::CONCAT) {
        NFA l = buildRec(node->left);
        NFA r = buildRec(node->right);
        l.end->transitions['$'].push_back(r.start);
        return {l.start, r.end};
    }
    if (node->token.type == TokenType::PLUS) {
        NFA nfa = buildRec(node->left);
        State* newStart = newState();
        State* newEnd = newState();
        newStart->transitions['$'].push_back(nfa.start);
        nfa.end->transitions['$'].push_back(nfa.start);
        nfa.end->transitions['$'].push_back(newEnd);
        return {newStart, newEnd};
    }
    if (node->token.type == TokenType::OPTION) {
        NFA nfa = buildRec(node->left);
        State* newStart = newState();
        State* newEnd = newState();
        newStart->transitions['$'].push_back(nfa.start);
        newStart->transitions['$'].push_back(newEnd);
        nfa.end->transitions['$'].push_back(newEnd);

        return {newStart, newEnd};
    }
    return {};
}


























