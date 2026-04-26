#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "include/Regex.h"
#include "include/DFAversion.h"
#include "include/StateElimination.h"
#include "include/Thompson.h"
#include "include/RegexParser.h"
#include "include/SyntaxTree.h"

int fileCounter = 1;

void printTree(Node* node, int depth = 0) {
    if (!node) return;
    printTree(node->right, depth + 1);
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << static_cast<int>(node->token.type) << ":" << node->token.value << "\n";
    printTree(node->left, depth + 1);
}

void saveToDot(const std::string& dotContent, std::string prefix) {
    std::string filename = prefix + "_" + std::to_string(fileCounter++) + ".dot";
    std::ofstream out(filename);
    if (out.is_open()) {
        out << dotContent;
        out.close();
        std::cout << "\nGraph saved to: " << std::filesystem::absolute(filename) << "\n";
    }
}

std::string getNfaDot(const NFA& nfa) {
    std::string res = "digraph NFA {\n  rankdir=LR;\n  node [shape=circle];\n";
    res += "  " + std::to_string(nfa.end->id) + " [shape=doublecircle];\n";
    std::set<State*> visited;
    std::vector<State*> stack = {nfa.start};
    while (!stack.empty()) {
        State* s = stack.back(); stack.pop_back();
        if (visited.count(s)) continue;
        visited.insert(s);
        for (const auto& [ch, targets] : s->transitions) {
            std::string lbl;
            if (ch == '$') lbl = "epsilon";
            else if (ch == '.') lbl = "any";
            else if (ch == '"') lbl = "\\\"";
            else lbl = std::string(1, ch);
            for (State* target : targets) {
                res += "  " + std::to_string(s->id) + " -> " + std::to_string(target->id) + " [label=\"" + lbl + "\"];\n";
                stack.push_back(target);
            }
        }
    }
    res += "  start [shape=point];\n  start -> " + std::to_string(nfa.start->id) + ";\n}\n";
    return res;
}

std::string getDfaDot(const DFA& dfa, std::string title) {
    std::string res = "digraph " + title + " {\n  rankdir=LR;\n";
    for (auto* s : dfa.states) {
        res += "  " + std::to_string(s->id) + (s->isFinal ? " [shape=doublecircle];\n" : " [shape=circle];\n");
    }
    for (auto* s : dfa.states) {
        for (const auto& [ch, target] : s->transitions) {
            std::string lbl = (ch == '.') ? "any" : std::string(1, ch);
            if (ch == '"') lbl = "\\\"";
            res += "  " + std::to_string(s->id) + " -> " + std::to_string(target->id) + " [label=\"" + lbl + "\"];\n";
        }
    }
    res += "  start [shape=point];\n  start -> " + std::to_string(dfa.start->id) + ";\n}\n";
    return res;
}



int main() {
    int choice;
    std::string regex, text;

    while (true) {
        std::cout << "1: Postfix (Парсинг)\n" << "2: Syntax Tree (Дерево)\n"
                  << "3: NFA (НКА по Томсону)\n" << "4: DFA (Обычный ДКА)\n" << "5: Minimized DFA (Минимизация)\n"
                  << "6: FindAll (Поиск и группы)\n" << "7: FindAll (Без доступа к группам)\n"
                  << "8: Complement (Дополнение)\n" << "9: Reverse (Реверс)\n"
                  << "10: State Elimination (В регулярку)\n" << "0: Exit\n";
        if (!(std::cin >> choice) || choice == 0) break;
        std::cout << "Enter regex: ";
        std::cin >> regex;
        DFAversion db;
        Thompson th;
        switch (choice) {
            case 1: {
                RegexParser p;
                auto pf = p.parse(regex);
                std::cout << "Postfix: ";
                for (auto& t : pf) std::cout << static_cast<int>(t.type) << " ";
                std::cout << "\n";
                break;
            }
            case 2: {
                RegexParser p;
                SyntaxTree st;
                std::cout << "Tree structure:\n";
                printTree(st.build(p.parse(regex)));
                break;
            }
            case 3: {
                saveToDot(getNfaDot(th.build(regex)), "nfa");
                break;
            }
            case 4: {
                NFA nfa = th.build(regex);
                DFA dfa = db.convert(nfa);
                saveToDot(getDfaDot(dfa, "RawDFA"), "dfa");
                break;
            }
            case 5: {
                NFA nfa = th.build(regex);
                DFA dfa = db.convert(nfa);
                DFA minDfa = db.minimize(dfa);
                saveToDot(getDfaDot(minDfa, "MinDFA"), "mindfa");
                break;
            }
            case 6: {
                std::cin.ignore();
                std::cout << "Enter line:\n";
                std::getline(std::cin, text);
                Regex re(regex);
                std::vector<Match> matches;
                re.findAll(text, matches);
                if (matches.empty()) std::cout << "No matches found.\n";
                for (auto& m : matches) {
                    std::cout << "Match: '" << m.value << "'\n";
                    for (auto const& [name, val] : m)
                        std::cout << "Group <" << name << ">: " << val << "\n";
                }
                break;
            }
            case 7: {
                std::cin.ignore();
                std::cout << "Enter line:\n";
                std::getline(std::cin, text);
                Regex re(regex);
                std::vector<std::string> results = re.findAll(text);
                if (results.empty()) {
                    std::cout << "No matches found.\n";
                }
                else {
                    std::cout << "Found strings:\n";
                    for (const std::string& s : results) {
                        std::cout << "\"" << s << "\"\n";
                    }
                }
                break;
            }
            case 8: {
                DFA dfa = db.convert(th.build(regex));
                DFA compDfa = db.complement(dfa);
                saveToDot(getDfaDot(compDfa, "CompDFA"), "complement");
                break;
            }
            case 9: {
                DFA dfa = db.convert(th.build(regex));
                DFA revDfa = db.reverse(dfa);
                saveToDot(getDfaDot(revDfa, "RevDFA"), "reverse");
                break;
            }
            case 10: {
                DFA minDfa = db.minimize(db.convert(th.build(regex)));
                StateElimination elim;
                std::cout << "Recovered Regex: " << elim.toRegex(minDfa) << "\n";
                break;
            }
            default:
                std::cout << "No such operation\n";
                break;
        }
    }
    return 0;
}