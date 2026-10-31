#include <iostream>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>

struct Node { int key = 0, parent = 0, left = 0, right = 0; bool red = false; };
class RedBlackTree {
    std::vector<Node> a{{}}; // 0 — чёрный NIL
    int root = 0;
    bool red(int x) const { return x && a[x].red; }
    void rotateLeft(int x) {
        int y = a[x].right; a[x].right = a[y].left;
        if (a[y].left) a[a[y].left].parent = x;
        a[y].parent = a[x].parent;
        if (!a[x].parent) root = y;
        else if (x == a[a[x].parent].left) a[a[x].parent].left = y;
        else a[a[x].parent].right = y;
        a[y].left = x; a[x].parent = y;
    }
    void rotateRight(int x) {
        int y = a[x].left; a[x].left = a[y].right;
        if (a[y].right) a[a[y].right].parent = x;
        a[y].parent = a[x].parent;
        if (!a[x].parent) root = y;
        else if (x == a[a[x].parent].right) a[a[x].parent].right = y;
        else a[a[x].parent].left = y;
        a[y].right = x; a[x].parent = y;
    }
public:
    bool contains(int key) const {
        int x = root;
        while (x && a[x].key != key) x = key < a[x].key ? a[x].left : a[x].right;
        return x != 0;
    }
    void insert(int key) {
        int p = 0, x = root;
        while (x) { p = x; if (key == a[x].key) return; x = key < a[x].key ? a[x].left : a[x].right; }
        int z = static_cast<int>(a.size()); a.push_back({key, p, 0, 0, true});
        if (!p) root = z;
        else if (key < a[p].key) a[p].left = z;
        else a[p].right = z;
        while (red(a[z].parent)) {
            int parent = a[z].parent, grand = a[parent].parent;
            if (parent == a[grand].left) {
                int uncle = a[grand].right;
                if (red(uncle)) { a[parent].red = a[uncle].red = false; a[grand].red = true; z = grand; }
                else { if (z == a[parent].right) { z = parent; rotateLeft(z); parent = a[z].parent; grand = a[parent].parent; }
                    a[parent].red = false; a[grand].red = true; rotateRight(grand); }
            } else {
                int uncle = a[grand].left;
                if (red(uncle)) { a[parent].red = a[uncle].red = false; a[grand].red = true; z = grand; }
                else { if (z == a[parent].left) { z = parent; rotateRight(z); parent = a[z].parent; grand = a[parent].parent; }
                    a[parent].red = false; a[grand].red = true; rotateLeft(grand); }
            }
        }
        a[root].red = false;
    }
};

// Обобщённое левостороннее красно-чёрное дерево (LLRB), вставка и поиск.
template <class Key, class Compare = std::less<Key>>
class RedBlackGeneric {
    struct Entry { Key key; bool red = true; Entry *l = nullptr, *r = nullptr; explicit Entry(const Key& x) : key(x) {} };
    std::vector<std::unique_ptr<Entry>> pool;
    Entry* root = nullptr;
    Compare comp;
    static bool red(Entry* p) { return p && p->red; }
    static Entry* left(Entry* p) {
        Entry* q = p->r; p->r = q->l; q->l = p; q->red = p->red; p->red = true; return q;
    }
    static Entry* right(Entry* p) {
        Entry* q = p->l; p->l = q->r; q->r = p; q->red = p->red; p->red = true; return q;
    }
    static void flipColors(Entry* p) {
        p->red = !p->red;
        p->l->red = !p->l->red;
        p->r->red = !p->r->red;
    }
    static Entry* fix(Entry* p) {
        if (red(p->r)) p = left(p);
        if (red(p->l) && red(p->l->l)) p = right(p);
        if (red(p->l) && red(p->r)) flipColors(p);
        return p;
    }
    static Entry* moveRedLeft(Entry* p) {
        flipColors(p);
        if (red(p->r->l)) { p->r = right(p->r); p = left(p); flipColors(p); }
        return p;
    }
    static Entry* moveRedRight(Entry* p) {
        flipColors(p);
        if (red(p->l->l)) { p = right(p); flipColors(p); }
        return p;
    }
    static Entry* minimum(Entry* p) { while (p->l) p = p->l; return p; }
    static Entry* removeMin(Entry* p) {
        if (!p->l) return nullptr;
        if (!red(p->l) && !red(p->l->l)) p = moveRedLeft(p);
        p->l = removeMin(p->l);
        return fix(p);
    }
    Entry* remove(Entry* p, const Key& x) {
        if (comp(x, p->key)) {
            if (!red(p->l) && !red(p->l->l)) p = moveRedLeft(p);
            p->l = remove(p->l, x);
        } else {
            if (red(p->l)) p = right(p);
            if (!comp(x, p->key) && !comp(p->key, x) && !p->r) return nullptr;
            if (!red(p->r) && !red(p->r->l)) p = moveRedRight(p);
            if (!comp(x, p->key) && !comp(p->key, x)) {
                p->key = minimum(p->r)->key;
                p->r = removeMin(p->r);
            } else p->r = remove(p->r, x);
        }
        return fix(p);
    }
    Entry* add(Entry* p, const Key& x) {
        if (!p) { pool.push_back(std::make_unique<Entry>(x)); return pool.back().get(); }
        if (comp(x, p->key)) p->l = add(p->l, x);
        else if (comp(p->key, x)) p->r = add(p->r, x);
        if (red(p->r) && !red(p->l)) p = left(p);
        if (red(p->l) && red(p->l->l)) p = right(p);
        if (red(p->l) && red(p->r)) { p->red = !p->red; p->l->red = false; p->r->red = false; }
        return p;
    }
    template <class OutputIt> static void emit(Entry* p, OutputIt& out) {
        if (!p) return; emit(p->l, out); *out++ = p->key; emit(p->r, out);
    }
public:
    void insert(const Key& x) { root = add(root, x); root->red = false; }
    void erase(const Key& x) {
        if (!contains(x)) return;
        if (!red(root->l) && !red(root->r)) root->red = true;
        root = remove(root, x);
        if (root) root->red = false;
    }
    bool contains(const Key& x) const {
        Entry* p = root;
        while (p) { if (comp(x, p->key)) p = p->l; else if (comp(p->key, x)) p = p->r; else return true; }
        return false;
    }
    template <class OutputIt> void copyInorder(OutputIt out) const { emit(root, out); }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; RedBlackGeneric<int> t;
    while (q--) { char op; int x; std::cin >> op >> x;
        if (op == '+') t.insert(x);
        else if (op == '-') t.erase(x);
        else if (op == '?') std::cout << (t.contains(x) ? "YES" : "NO") << '\n';
    }
}
