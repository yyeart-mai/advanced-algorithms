#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
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

// Обобщённое декартово дерево с псевдослучайными приоритетами.
template <class Key, class Compare = std::less<Key>>
class TreapGeneric {
    struct Entry { Key key; std::uint32_t priority; Entry *l = nullptr, *r = nullptr; Entry(const Key& x, std::uint32_t p) : key(x), priority(p) {} };
    std::vector<std::unique_ptr<Entry>> pool;
    Entry* root = nullptr;
    Compare comp;
    std::uint32_t state = 2463534242u;
    std::uint32_t randomPriority() { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; }
    static Entry* right(Entry* p) { Entry* q = p->l; p->l = q->r; q->r = p; return q; }
    static Entry* left(Entry* p) { Entry* q = p->r; p->r = q->l; q->l = p; return q; }
    Entry* add(Entry* p, const Key& x) {
        if (!p) { pool.push_back(std::make_unique<Entry>(x, randomPriority())); return pool.back().get(); }
        if (comp(x, p->key)) { p->l = add(p->l, x); if (p->l->priority > p->priority) p = right(p); }
        else if (comp(p->key, x)) { p->r = add(p->r, x); if (p->r->priority > p->priority) p = left(p); }
        return p;
    }
    static Entry* merge(Entry* a, Entry* b) {
        if (!a) return b; if (!b) return a;
        if (a->priority > b->priority) { a->r = merge(a->r, b); return a; }
        b->l = merge(a, b->l); return b;
    }
    Entry* remove(Entry* p, const Key& x) {
        if (!p) return nullptr;
        if (comp(x, p->key)) p->l = remove(p->l, x);
        else if (comp(p->key, x)) p->r = remove(p->r, x);
        else return merge(p->l, p->r);
        return p;
    }
    template <class OutputIt> static void emit(Entry* p, OutputIt& out) {
        if (!p) return; emit(p->l, out); *out++ = p->key; emit(p->r, out);
    }
public:
    void insert(const Key& x) { root = add(root, x); }
    void erase(const Key& x) { root = remove(root, x); }
    bool contains(const Key& x) const {
        Entry* p = root;
        while (p) { if (comp(x, p->key)) p = p->l; else if (comp(p->key, x)) p = p->r; else return true; }
        return false;
    }
    template <class OutputIt> void copyInorder(OutputIt out) const { emit(root, out); }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; TreapGeneric<int> t;
    while (q--) { char op; int x; std::cin >> op >> x;
        if (op == '+') t.insert(x);
        else if (op == '-') t.erase(x);
        else if (op == '?') std::cout << (t.contains(x) ? "YES" : "NO") << '\n';
    }
}
