#include "../include/SyntaxTree.h"
#include <stack>

Node* SyntaxTree::build(const std::vector<Token>& postfix) {
    std::stack<Node*> st;
    for (const auto& token : postfix) {
        if (token.type == TokenType::SYMBOL || token.type == TokenType::DOT) {
            st.push(new Node(token));
        }
        else if (token.type == TokenType::PLUS || token.type == TokenType::OPTION || token.type == TokenType::REPEAT) {
            if (st.empty()) return nullptr;
            Node* node = new Node(token);
            node->left = st.top();
            st.pop();
            st.push(node);
        }
        else {
            if (st.size() < 2) return nullptr;
            Node* node = new Node(token);
            node->right = st.top();
            st.pop();
            node->left = st.top();
            st.pop();
            st.push(node);
        }
    }
    if (st.size() != 1) return nullptr;
    return st.top();
}