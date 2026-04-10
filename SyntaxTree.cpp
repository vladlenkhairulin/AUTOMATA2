#include "SyntaxTree.h"
#include <stack>

Node* SyntaxTree::build(const std::vector<Token>& postfix) {
    std::stack<Node*> st;
    for (const auto& token : postfix) {
        if (token.type == TokenType::SYMBOL) {
            st.push(new Node(token));
        }
        else if (token.type == TokenType::PLUS || token.type == TokenType::OPTION) {
            Node* node = new Node(token);
            node->left = st.top();
            st.pop();
            st.push(node);
        }
        else {
            Node* node = new Node(token);
            node->right = st.top();
            st.pop();
            node->left = st.top();
            st.pop();
            st.push(node);
        }
    }
    return st.top();
}