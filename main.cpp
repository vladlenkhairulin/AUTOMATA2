#include <iostream>
#include "RegexParser.h"
#include "SyntaxTree.h"
#include "Thompson.h"
#include "DFAversion.h"
#include "StateElimination.h"
#include <string>
#include <fstream>
#include <set>
#include <vector>

void printTree(Node* node, int depth = 0) {
    if (!node) return;
    printTree(node->right, depth + 1);
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << static_cast<int>(node->token.type) << ":" << node->token.value << "\n";
    printTree(node->left, depth + 1);
}

void saveNFAtoDot(const NFA& nfa, const std::string& filename = "nfa.dot") {
    std::ofstream out(filename);
    if (!out) return;
    out << "digraph NFA {\n";
    out << "    rankdir=LR;\n";
    out << "    node [shape=circle];\n";
    out << "    " << nfa.end->id << " [shape=doublecircle];\n";
    std::set<State*> visited;
    std::vector<State*> stack = {nfa.start};

    while (!stack.empty()) {
        State* s = stack.back();
        stack.pop_back();
        if (visited.count(s)) continue;
        visited.insert(s);

        for (const auto& trans : s->transitions) {
            char label = trans.first;
            std::string lbl = (label == '$') ? "ε" : (label == '.') ? "." : std::string(1, label);

            for (State* target : trans.second) {
                out << "    " << s->id << " -> " << target->id
                    << " [label=\"" << lbl << "\"];\n";
                stack.push_back(target);
            }
        }
    }

    out << "    start [shape=point];\n";
    out << "    start -> " << nfa.start->id << ";\n";
    out << "}\n";
    std::cout << "NFA saved to " << filename << "\n";
}

void saveDFAtoDot(const DFA& dfa, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return;

    out << "digraph DFA {\n";
    out << "    rankdir=LR;\n";
    for (auto* s : dfa.states) {
        out << "    " << s->id;
        if (s->isFinal) out << " [shape=doublecircle]";
        out << ";\n";
    }
    for (auto* s : dfa.states) {
        for (const auto& t : s->transitions) {
            std::string label = (t.first == '.') ? "." : std::string(1, t.first);
            out << "    " << s->id << " -> " << t.second->id
                << " [label=\"" << label << "\"];\n";
        }
    }
    out << "    start [shape=point];\n";
    out << "    start -> " << dfa.start->id << ";\n";
    out << "}\n";
    std::cout << "Saved to " << filename << "\n";
}

int main() {
    std::string regex;
    std::cout << "Enter regex: ";
    std::cin >> regex;

    RegexParser parser;
    auto postfix = parser.parse(regex);
    SyntaxTree tree;
    Node* root = tree.build(postfix);
    Thompson thompson;
    NFA nfa = thompson.build(regex);
    DFAversion dfaBuilder;
    DFA dfa = dfaBuilder.convert(nfa);
    DFA minDFA = dfaBuilder.minimize(dfa);
    StateElimination elim;
    std::string recovered = elim.toRegex(minDFA);

    int choice;
    std::cout << "\n1 - Postfix\n2 - Tree\n3 - NFA\n4 - DFA\n5 - Min DFA\n6 - Recovered Regex\n7 - All\nChoice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "Postfix:\n";
            for (const auto& t : postfix) {
                std::cout << static_cast<int>(t.type) << t.value << "\n";
            }
            break;

        case 2:
            std::cout << "Syntax Tree:\n";
            printTree(root);
            break;

        case 3:
            std::cout << "NFA: start=" << nfa.start->id << ", end=" << nfa.end->id << "\n";
            saveNFAtoDot(nfa, "nfa.dot");
            break;

        case 4:
            std::cout << "DFA states: " << dfa.states.size() << "\n";
            saveDFAtoDot(dfa, "dfa.dot");
            break;

        case 5:
            std::cout << "Min DFA states: " << minDFA.states.size() << "\n";
            saveDFAtoDot(minDFA, "mindfa.dot");
            break;

        case 6:
            std::cout << "Recovered regex: " << recovered << "\n";
            break;

        case 7:
            std::cout << "Postfix:\n";
            for (const auto& t : postfix) std::cout << static_cast<int>(t.type) << t.value << "\n";

            std::cout << "\nSyntax Tree:\n";
            printTree(root);

            std::cout << "\nNFA: start=" << nfa.start->id << ", end=" << nfa.end->id << "\n";

            std::cout << "\nDFA states: " << dfa.states.size() << "\n";
            saveDFAtoDot(dfa, "dfa.dot");

            std::cout << "\nMin DFA states: " << minDFA.states.size() << "\n";
            saveDFAtoDot(minDFA, "mindfa.dot");

            std::cout << "\nRecovered regex: " << recovered << "\n";
            break;

        default:
            std::cout << "Invalid choice\n";
    }

    return 0;
}