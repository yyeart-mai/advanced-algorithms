#include <iostream>
#include <memory>
#include <vector>

// B-дерево минимальной степени 2: в узле 1..3 ключа (кроме пустого корня).
struct Page {
    std::vector<int> keys;
    std::vector<std::unique_ptr<Page>> children;
    bool leaf() const { return children.empty(); }
};
class BTree {
    std::unique_ptr<Page> root = std::make_unique<Page>();
    static bool contains(const Page* p, int x) {
        std::size_t i = 0;
        while (i < p->keys.size() && p->keys[i] < x) ++i;
        if (i < p->keys.size() && p->keys[i] == x) return true;
        return !p->leaf() && contains(p->children[i].get(), x);
    }
    static void split(Page* p, std::size_t i) {
        auto& child = p->children[i];
        auto sibling = std::make_unique<Page>();
        int middle = child->keys[1];
        sibling->keys.push_back(child->keys[2]);
        child->keys.resize(1);
        if (!child->leaf()) {
            sibling->children.push_back(std::move(child->children[2]));
            sibling->children.push_back(std::move(child->children[3]));
            child->children.resize(2);
        }
        p->keys.insert(p->keys.begin() + i, middle);
        p->children.insert(p->children.begin() + i + 1, std::move(sibling));
    }
    static void insertNonFull(Page* p, int x) {
        std::size_t i = 0;
        while (i < p->keys.size() && p->keys[i] < x) ++i;
        if (p->leaf()) { p->keys.insert(p->keys.begin() + i, x); return; }
        if (p->children[i]->keys.size() == 3) {
            split(p, i);
            if (x > p->keys[i]) ++i;
        }
        insertNonFull(p->children[i].get(), x);
    }
public:
    bool contains(int x) const { return contains(root.get(), x); }
    void insert(int x) {
        if (contains(x)) return;
        if (root->keys.size() == 3) {
            auto newRoot = std::make_unique<Page>();
            newRoot->children.push_back(std::move(root));
            split(newRoot.get(), 0);
            root = std::move(newRoot);
        }
        insertNonFull(root.get(), x);
    }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; BTree t;
    while (q--) { char op; int x; std::cin >> op >> x; if (op == '+') t.insert(x); else std::cout << (t.contains(x) ? "YES" : "NO") << '\n'; }
}
