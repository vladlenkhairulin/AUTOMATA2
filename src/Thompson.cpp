#include "Thompson.h"

#include <stdexcept>

State* Thompson::newState() {
    State* state = new State();
    state->id = counter++;
    return state;
}

NFA Thompson::buildStar(Node* node) {
    NFA sub = buildRec(node);
    State* newStart = newState();
    State* newEnd = newState();
    newStart->transitions['$'].push_back(newEnd);
    newStart->transitions['$'].push_back(sub.start);
    sub.end->transitions['$'].push_back(newEnd);
    sub.end->transitions['$'].push_back(sub.start);
    return {newStart, newEnd};
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
    if (node->token.type == TokenType::REPEAT) {
        std::string v = node->token.value;
        size_t commaPos = v.find(',');
        int minRep = 0;
        int maxRep = -1;
        if (commaPos == std::string::npos) {
            minRep = maxRep = std::stoi(v);
        }
        else {
            std::string left = v.substr(0, commaPos);
            std::string right = v.substr(commaPos + 1);
            minRep = left.empty()? 0 : std::stoi(left);
            maxRep = right.empty()? -1 : std::stoi(right);
        }
        if (maxRep != -1 && maxRep < minRep) {
            throw std::invalid_argument("Invalid repeat range");

        }
        NFA res;
        if (minRep == 0) {
            State* s1 = newState();
            State* s2 = newState();
            s1->transitions['$'].push_back(s2);
            res = {s1,s2};
        }
        else {
            res = buildRec(node->left);
            for (int i = 1; i < minRep; i++) {
                NFA nextCopy = buildRec(node->left);
                res.end->transitions['$'].push_back(nextCopy.start);
                res.end = nextCopy.end;
            }
        }
        if (maxRep == -1) {
            NFA star = buildStar(node->left);
            res.end->transitions['$'].push_back(star.start);
            res.end = star.end;
        }
        else if (maxRep > minRep) {
            int dif = maxRep - minRep;
            for (int i = 0; i<dif; i++) {
                NFA opt = buildRec(node->left);
                State* skipEnd = newState();
                res.end->transitions['$'].push_back(opt.start);
                opt.end->transitions['$'].push_back(skipEnd);
                res.end->transitions['$'].push_back(skipEnd);
                res.end = skipEnd;
            }
        }
        return res;
    }
    if (node->token.type == TokenType::GRP) {
        NFA rec = buildRec(node->left);
        State* s1 = newState();
        State* s2 = newState();
        s1->startTag = node->token.value;
        s2->endTag = node->token.value;
        s1->transitions['$'].push_back(rec.start);
        rec.end->transitions['$'].push_back(s2);
        return {s1, s2};
    }
    if (node->token.type == TokenType::GRPREF) {
        State* s1 = newState();
        State* s2 = newState();
        s1->refTag = node->token.value;
        s1->transitions['$'].push_back(s2);
        return {s1, s2};
    }
    return {};
}

























