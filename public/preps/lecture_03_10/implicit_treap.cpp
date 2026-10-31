#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Позиция элемента определяется размером левого поддерева, а не ключом.
// T поддерживает сложение и нулевое значение.
template <class T>
class ImplicitTreap {
    struct Node {
        T value, sum;
        int size = 1;
        std::uint32_t priority;
        bool reversed = false;
        Node *left = nullptr, *right = nullptr;
        Node(const T& x, std::uint32_t p) : value(x), sum(x), priority(p) {}
    };
    std::vector<std::unique_ptr<Node>> pool;
    Node* root = nullptr;
    std::uint32_t state = 2463534242u;
    std::uint32_t randomPriority() { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; }
    static int size(Node* p) { return p ? p->size : 0; }
    static T sum(Node* p) { return p ? p->sum : T{}; }
    static void push(Node* p) {
        if (!p || !p->reversed) return;
        std::swap(p->left, p->right);
        if (p->left) p->left->reversed ^= 1;
        if (p->right) p->right->reversed ^= 1;
        p->reversed = false;
    }
    static void update(Node* p) {
        if (p) { p->size = 1 + size(p->left) + size(p->right); p->sum = p->value + sum(p->left) + sum(p->right); }
    }
    static std::pair<Node*, Node*> split(Node* p, int leftSize) {
        if (!p) return {nullptr, nullptr};
        push(p);
        if (size(p->left) >= leftSize) {
            auto [a, b] = split(p->left, leftSize); p->left = b; update(p); return {a, p};
        }
        auto [a, b] = split(p->right, leftSize - size(p->left) - 1);
        p->right = a; update(p); return {p, b};
    }
    static Node* merge(Node* a, Node* b) {
        if (!a) return b; if (!b) return a;
        if (a->priority > b->priority) { push(a); a->right = merge(a->right, b); update(a); return a; }
        push(b); b->left = merge(a, b->left); update(b); return b;
    }
    template <class OutputIt> static void emit(Node* p, OutputIt& out) {
        if (!p) return;
        push(p); emit(p->left, out); *out++ = p->value; emit(p->right, out);
    }
public:
    int size() const { return size(root); }
    void insert(int pos, const T& value) {
        pool.push_back(std::make_unique<Node>(value, randomPriority()));
        auto [a, b] = split(root, pos);
        root = merge(merge(a, pool.back().get()), b);
    }
    void erase(int pos) {
        auto [a, b] = split(root, pos);
        auto [discard, c] = split(b, 1);
        (void)discard; // Память узла остаётся в pool до уничтожения дерева.
        root = merge(a, c);
    }
    void reverse(int l, int r) {
        auto [a, b] = split(root, l); auto [middle, c] = split(b, r - l);
        if (middle) middle->reversed ^= 1;
        root = merge(a, merge(middle, c));
    }
    T rangeSum(int l, int r) {
        auto [a, b] = split(root, l); auto [middle, c] = split(b, r - l);
        T answer = sum(middle);
        root = merge(a, merge(middle, c));
        return answer;
    }
    template <class InputIt> void append(InputIt first, InputIt last) {
        for (; first != last; ++first) insert(size(), *first);
    }
    template <class OutputIt> void copyTo(OutputIt out) { emit(root, out); }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    ImplicitTreap<long long> tree;
    while (q--) {
        std::string op; std::cin >> op;
        if (op == "insert") { int p; long long x; std::cin >> p >> x; tree.insert(p, x); }
        else if (op == "erase") { int p; std::cin >> p; tree.erase(p); }
        else { int l, r; std::cin >> l >> r;
            if (op == "reverse") tree.reverse(l, r);
            else if (op == "sum") std::cout << tree.rangeSum(l, r) << '\n';
        }
    }
}
