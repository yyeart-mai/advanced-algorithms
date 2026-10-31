#include <iostream>
#include <memory>
#include <string>
#include <vector>

// 2-3-4 дерево — B-дерево минимальной степени 2 (1..3 ключа в узле).
class TwoThreeFour {
    struct Node {
        std::vector<int> keys;
        std::vector<std::unique_ptr<Node>> child;
        bool leaf() const { return child.empty(); }
    };
    std::unique_ptr<Node> root = std::make_unique<Node>();
    static void split(Node* p, std::size_t i) {
        auto& left = p->child[i];
        auto right = std::make_unique<Node>();
        int middle = left->keys[1];
        right->keys.push_back(left->keys[2]);
        left->keys.resize(1);
        if (!left->leaf()) {
            right->child.push_back(std::move(left->child[2]));
            right->child.push_back(std::move(left->child[3]));
            left->child.resize(2);
        }
        p->keys.insert(p->keys.begin() + i, middle);
        p->child.insert(p->child.begin() + i + 1, std::move(right));
    }
    static void add(Node* p, int x) {
        std::size_t i = 0;
        while (i < p->keys.size() && p->keys[i] < x) ++i;
        if (p->leaf()) { p->keys.insert(p->keys.begin() + i, x); return; }
        if (p->child[i]->keys.size() == 3) {
            split(p, i);
            if (x > p->keys[i]) ++i;
        }
        add(p->child[i].get(), x);
    }
public:
    bool contains(int x) const {
        auto* p = root.get();
        while (p) {
            std::size_t i = 0;
            while (i < p->keys.size() && p->keys[i] < x) ++i;
            if (i < p->keys.size() && p->keys[i] == x) return true;
            if (p->leaf()) return false;
            p = p->child[i].get();
        }
        return false;
    }
    void insert(int x) {
        if (contains(x)) return;
        if (root->keys.size() == 3) {
            auto newRoot = std::make_unique<Node>();
            newRoot->child.push_back(std::move(root));
            split(newRoot.get(), 0);
            root = std::move(newRoot);
        }
        add(root.get(), x);
    }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    TwoThreeFour tree;
    while (q--) {
        char op; int x; std::cin >> op >> x;
        if (op == '+') tree.insert(x);
        else if (op == '?') std::cout << (tree.contains(x) ? "YES" : "NO") << '\n';
    }
}
