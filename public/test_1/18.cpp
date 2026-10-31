#include <climits>

struct Node {
    int value;
    Node* left;
    Node* right;
};

int findMax(Node *root) {
    if (!root) return INT_MIN;
    
    Node *current = root;
    while (current->right) {
        current = current->right;
    }
    
    return current->value;
}
