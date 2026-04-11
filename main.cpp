#include <iostream>
#include "RegexParser.h"
#include "SyntaxTree.h"
#include "Thompson.h"
#include "DFAversion.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
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
        Thompson thompson;
        NFA nfa = thompson.build(regex);
        std::cout << "\nNFA built successfully\n";
        DFAversion dfaBuilder;
        DFA dfa = dfaBuilder.convert(nfa);
        std::cout << "\nDFA built successfully\n";

    }
    catch (const std::exception& e) {
        std::cout << "error: " << e.what() << "\n";
    }
    return 0;
}