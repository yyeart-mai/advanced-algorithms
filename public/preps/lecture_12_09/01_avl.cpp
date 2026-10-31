#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
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

// Обобщённое AVL: Key копируемый, Compare задаёт строгий порядок.
template <class Key, class Compare = std::less<Key>>
class AVLGeneric {
    struct Entry { Key key; int h = 1; Entry *l = nullptr, *r = nullptr; explicit Entry(const Key& x) : key(x) {} };
    std::vector<std::unique_ptr<Entry>> pool;
    Entry* root = nullptr;
    Compare comp;
    static int h(Entry* p) { return p ? p->h : 0; }
    static void update(Entry* p) { p->h = 1 + std::max(h(p->l), h(p->r)); }
    static Entry* right(Entry* p) { Entry* q = p->l; p->l = q->r; q->r = p; update(p); update(q); return q; }
    static Entry* left(Entry* p) { Entry* q = p->r; p->r = q->l; q->l = p; update(p); update(q); return q; }
    static Entry* balance(Entry* p) {
        update(p);
        if (h(p->l) - h(p->r) == 2) { if (h(p->l->r) > h(p->l->l)) p->l = left(p->l); return right(p); }
        if (h(p->r) - h(p->l) == 2) { if (h(p->r->l) > h(p->r->r)) p->r = right(p->r); return left(p); }
        return p;
    }
    Entry* add(Entry* p, const Key& x) {
        if (!p) { pool.push_back(std::make_unique<Entry>(x)); return pool.back().get(); }
        if (comp(x, p->key)) p->l = add(p->l, x);
        else if (comp(p->key, x)) p->r = add(p->r, x);
        return balance(p);
    }
    static Entry* removeMin(Entry* p, Entry*& minimum) {
        if (!p->l) { minimum = p; return p->r; }
        p->l = removeMin(p->l, minimum);
        return balance(p);
    }
    Entry* remove(Entry* p, const Key& x) {
        if (!p) return nullptr;
        if (comp(x, p->key)) p->l = remove(p->l, x);
        else if (comp(p->key, x)) p->r = remove(p->r, x);
        else {
            Entry* l = p->l; Entry* r = p->r;
            if (!r) return l;
            Entry* minimum = nullptr;
            r = removeMin(r, minimum);
            minimum->l = l; minimum->r = r;
            return balance(minimum);
        }
        return balance(p);
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
    int q; if (!(std::cin >> q)) return 0; AVLGeneric<int> t;
    while (q--) { char op; int x; std::cin >> op >> x;
        if (op == '+') t.insert(x);
        else if (op == '-') t.erase(x);
        else if (op == '?') std::cout << (t.contains(x) ? "YES" : "NO") << '\n';
    }
}
