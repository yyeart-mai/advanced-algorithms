#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

struct Node { int key, height = 1; Node *left = nullptr, *right = nullptr; explicit Node(int x) : key(x) {} };
class AVL {
    std::vector<std::unique_ptr<Node>> nodes;
    Node* root = nullptr;
    static int height(Node* p) { return p ? p->height : 0; }
    static void update(Node* p) { p->height = 1 + std::max(height(p->left), height(p->right)); }
    static Node* rotateRight(Node* p) {
        Node* q = p->left; p->left = q->right; q->right = p; update(p); update(q); return q;
    }
    static Node* rotateLeft(Node* p) {
        Node* q = p->right; p->right = q->left; q->left = p; update(p); update(q); return q;
    }
    static Node* balance(Node* p) {
        update(p);
        if (height(p->left) - height(p->right) == 2) {
            if (height(p->left->right) > height(p->left->left)) p->left = rotateLeft(p->left);
            return rotateRight(p);
        }
        if (height(p->right) - height(p->left) == 2) {
            if (height(p->right->left) > height(p->right->right)) p->right = rotateRight(p->right);
            return rotateLeft(p);
        }
        return p;
    }
    Node* insert(Node* p, int x) {
        if (!p) { nodes.push_back(std::make_unique<Node>(x)); return nodes.back().get(); }
        if (x < p->key) p->left = insert(p->left, x);
        else if (x > p->key) p->right = insert(p->right, x);
        return balance(p);
    }
public:
    void insert(int x) { root = insert(root, x); }
    bool contains(int x) const {
        Node* p = root;
        while (p && p->key != x) p = x < p->key ? p->left : p->right;
        return p != nullptr;
    }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; AVL t;
    while (q--) { char op; int x; std::cin >> op >> x; if (op == '+') t.insert(x); else std::cout << (t.contains(x) ? "YES" : "NO") << '\n'; }
}
