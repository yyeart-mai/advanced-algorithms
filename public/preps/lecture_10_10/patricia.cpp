#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <type_traits>

// Двоичный PATRICIA для целых ключей фиксированной ширины.
template <class UInt>
class Patricia {
    static_assert(std::is_unsigned_v<UInt>, "unsigned key required");
    struct Node {
        UInt key{};
        int bit = -1; // -1 означает лист.
        std::unique_ptr<Node> child[2];
        explicit Node(UInt x) : key(x) {}
        explicit Node(int b) : bit(b) {}
    };
    std::unique_ptr<Node> root;
    static int at(UInt x, int bit) { return int((x >> bit) & UInt(1)); }
    static int highest(UInt x) {
        for (int bit = std::numeric_limits<UInt>::digits - 1; bit >= 0; --bit)
            if (at(x, bit)) return bit;
        return -1;
    }
    static void add(std::unique_ptr<Node>& p, UInt x, int differentBit) {
        if (p->bit < differentBit) {
            auto branch = std::make_unique<Node>(differentBit);
            int side = at(x, differentBit);
            branch->child[side] = std::make_unique<Node>(x);
            branch->child[1 - side] = std::move(p);
            p = std::move(branch);
        } else add(p->child[at(x, p->bit)], x, differentBit);
    }
    static bool erase(std::unique_ptr<Node>& p, UInt x) {
        if (p->bit == -1) { if (p->key != x) return false; p.reset(); return true; }
        int side = at(x, p->bit);
        if (!erase(p->child[side], x)) return false;
        if (!p->child[side]) {
            auto sibling = std::move(p->child[1 - side]);
            p = std::move(sibling);
        }
        return true;
    }
public:
    bool contains(UInt x) const {
        const Node* p = root.get();
        while (p && p->bit >= 0) p = p->child[at(x, p->bit)].get();
        return p && p->key == x;
    }
    void insert(UInt x) {
        if (!root) { root = std::make_unique<Node>(x); return; }
        const Node* leaf = root.get();
        while (leaf->bit >= 0) leaf = leaf->child[at(x, leaf->bit)].get();
        if (leaf->key == x) return;
        add(root, x, highest(x ^ leaf->key));
    }
    bool erase(UInt x) { return root && erase(root, x); }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    Patricia<std::uint32_t> trie;
    while (q--) {
        char op; std::uint32_t x; std::cin >> op >> x;
        if (op == '+') trie.insert(x);
        else if (op == '-') trie.erase(x);
        else if (op == '?') std::cout << (trie.contains(x) ? "YES" : "NO") << '\n';
    }
}
