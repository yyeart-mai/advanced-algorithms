#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// Учебное несбалансированное BST: в худшем случае высота O(n).
template <class Key, class Compare = std::less<Key>>
class BST {
    struct Node {
        Key key;
        std::unique_ptr<Node> left, right;
        explicit Node(const Key& x) : key(x) {}
    };
    std::unique_ptr<Node> root;
    Compare comp;
    static void rotateLeft(std::unique_ptr<Node>& p) {
        auto q = std::move(p->right);
        p->right = std::move(q->left);
        q->left = std::move(p);
        p = std::move(q);
    }
    static void rotateRight(std::unique_ptr<Node>& p) {
        auto q = std::move(p->left);
        p->left = std::move(q->right);
        q->right = std::move(p);
        p = std::move(q);
    }
    void insert(std::unique_ptr<Node>& p, const Key& x) {
        if (!p) { p = std::make_unique<Node>(x); return; }
        if (comp(x, p->key)) insert(p->left, x);
        else if (comp(p->key, x)) insert(p->right, x);
    }
    void erase(std::unique_ptr<Node>& p, const Key& x) {
        if (!p) return;
        if (comp(x, p->key)) erase(p->left, x);
        else if (comp(p->key, x)) erase(p->right, x);
        else if (!p->left) p = std::move(p->right);
        else if (!p->right) p = std::move(p->left);
        else {
            auto* successor = &p->right;
            while ((*successor)->left) successor = &(*successor)->left;
            p->key = (*successor)->key;
            *successor = std::move((*successor)->right);
        }
    }
    template <class OutputIt>
    static void inorder(const Node* p, OutputIt& out) {
        if (!p) return;
        inorder(p->left.get(), out);
        *out++ = p->key;
        inorder(p->right.get(), out);
    }
public:
    void insert(const Key& x) { insert(root, x); }
    void erase(const Key& x) { erase(root, x); }
    bool contains(const Key& x) const {
        auto* p = root.get();
        while (p) {
            if (comp(x, p->key)) p = p->left.get();
            else if (comp(p->key, x)) p = p->right.get();
            else return true;
        }
        return false;
    }
    bool rotateRootLeft() { if (!root || !root->right) return false; rotateLeft(root); return true; }
    bool rotateRootRight() { if (!root || !root->left) return false; rotateRight(root); return true; }
    template <class OutputIt> void copyInorder(OutputIt out) const { inorder(root.get(), out); }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    BST<int> tree;
    while (q--) {
        std::string command; std::cin >> command;
        if (command == "rotate_left") tree.rotateRootLeft();
        else if (command == "rotate_right") tree.rotateRootRight();
        else { int x; std::cin >> x;
            if (command == "insert") tree.insert(x);
            else if (command == "delete") tree.erase(x);
            else if (command == "find") std::cout << (tree.contains(x) ? "YES" : "NO") << '\n';
        }
    }
}
