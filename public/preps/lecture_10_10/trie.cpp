#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>

template <class Symbol>
class Trie {
    struct Node { bool terminal = false; std::map<Symbol, std::unique_ptr<Node>> next; };
    Node root;
    template <class It>
    static bool erase(Node& node, It first, It last) {
        if (first == last) { if (!node.terminal) return false; node.terminal = false; return true; }
        auto edge = node.next.find(*first);
        if (edge == node.next.end()) return false;
        bool removed = erase(*edge->second, std::next(first), last);
        if (removed && !edge->second->terminal && edge->second->next.empty()) node.next.erase(edge);
        return removed;
    }
public:
    template <class It> void insert(It first, It last) {
        Node* p = &root;
        for (; first != last; ++first) {
            auto& next = p->next[*first];
            if (!next) next = std::make_unique<Node>();
            p = next.get();
        }
        p->terminal = true;
    }
    template <class It> bool contains(It first, It last) const {
        const Node* p = &root;
        for (; first != last; ++first) {
            auto edge = p->next.find(*first);
            if (edge == p->next.end()) return false;
            p = edge->second.get();
        }
        return p->terminal;
    }
    template <class It> bool erase(It first, It last) { return erase(root, first, last); }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    Trie<char> trie;
    while (q--) {
        char op; std::string word; std::cin >> op >> word;
        if (op == '+') trie.insert(word.begin(), word.end());
        else if (op == '-') trie.erase(word.begin(), word.end());
        else if (op == '?') std::cout << (trie.contains(word.begin(), word.end()) ? "YES" : "NO") << '\n';
    }
}
