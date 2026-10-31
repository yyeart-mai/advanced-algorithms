#include <iostream>
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
int main() {
    int q; if (!(std::cin >> q)) return 0; RedBlackTree t;
    while (q--) { char op; int x; std::cin >> op >> x; if (op == '+') t.insert(x); else std::cout << (t.contains(x) ? "YES" : "NO") << '\n'; }
}
