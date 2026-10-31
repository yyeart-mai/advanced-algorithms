#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

struct Node { int key; std::uint32_t priority; Node *left = nullptr, *right = nullptr; Node(int k, std::uint32_t p) : key(k), priority(p) {} };
class Treap {
    std::vector<std::unique_ptr<Node>> nodes;
    Node* root = nullptr;
    std::uint32_t state = 2463534242u;
    std::uint32_t randomPriority() { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; }
    static Node* right(Node* p) { Node* q = p->left; p->left = q->right; q->right = p; return q; }
    static Node* left(Node* p) { Node* q = p->right; p->right = q->left; q->left = p; return q; }
    Node* insert(Node* p, int x) {
        if (!p) { nodes.push_back(std::make_unique<Node>(x, randomPriority())); return nodes.back().get(); }
        if (x < p->key) { p->left = insert(p->left, x); if (p->left->priority > p->priority) p = right(p); }
        else if (x > p->key) { p->right = insert(p->right, x); if (p->right->priority > p->priority) p = left(p); }
        return p;
    }
public:
    void insert(int x) { root = insert(root, x); }
    bool contains(int x) const { Node* p = root; while (p && p->key != x) p = x < p->key ? p->left : p->right; return p != nullptr; }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; Treap t;
    while (q--) { char op; int x; std::cin >> op >> x; if (op == '+') t.insert(x); else std::cout << (t.contains(x) ? "YES" : "NO") << '\n'; }
}
