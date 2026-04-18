#include <iostream>
#include "RegexParser.h"
#include "SyntaxTree.h"
#include "Thompson.h"
#include "DFAversion.h"

void print(Node* node, int depth = 0) {
    if (!node) return;
    print(node->right, depth + 1);
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << static_cast<int>(node->token.type) << ":" << node->token.value << "\n";
    print(node->left, depth + 1);
}

int main() {

    try {
        std::string regex;
        std::cout << "Enter regex:\n";
        std::cin >> regex;
        RegexParser parser;
        std::vector<Token> postfix = parser.parse(regex);
        std::cout << "Postfix form:\n";
        for (const auto& token: postfix) {
            std::cout << static_cast<int>(token.type) << token.value << "\n";
        }
        SyntaxTree tree;
        Node* root = tree.build(postfix);
        std::cout << "Syntax Tree:\n";
        print(root);

        Thompson thompson;
        NFA nfa = thompson.build(regex);
        std::cout << "NFA start id: " << nfa.start->id << "\n";
        std::cout << "NFA end id: " << nfa.end->id << "\n";
        std::cout << "\nNFA built successfully\n";

        DFAversion dfaBuilder;
        DFA dfa = dfaBuilder.convert(nfa);
        std::cout << "\nDFA built successfully\n";
        std::cout << "DFA states number: " << dfa.states.size() << "\n";

    }
    catch (const std::exception& e) {
        std::cout << "error: " << e.what() << "\n";
    }
    return 0;
}