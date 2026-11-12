#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Простое посимвольное дерево для строк. После удаления пустые вершины
// остаются в массиве: это не влияет на поиск и упрощает первую реализацию.
class SimpleTrie {
    struct Node {
        bool terminal = false;
        std::map<char, int> next;
    };
    std::vector<Node> nodes = std::vector<Node>(1);
public:
    void insert(const std::string& word) {
        int p = 0;
        for (char c : word) {
            auto it = nodes[p].next.find(c);
            if (it == nodes[p].next.end()) {
                int child = static_cast<int>(nodes.size());
                nodes[p].next[c] = child;
                nodes.emplace_back();
                p = child;
            } else p = it->second;
        }
        nodes[p].terminal = true;
    }
    bool contains(const std::string& word) const {
        int p = 0;
        for (char c : word) {
            auto it = nodes[p].next.find(c);
            if (it == nodes[p].next.end()) return false;
            p = it->second;
        }
        return nodes[p].terminal;
    }
    bool erase(const std::string& word) {
        int p = 0;
        for (char c : word) {
            auto it = nodes[p].next.find(c);
            if (it == nodes[p].next.end()) return false;
            p = it->second;
        }
        bool existed = nodes[p].terminal;
        nodes[p].terminal = false;
        return existed;
    }
};

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
    SimpleTrie trie;
    while (q--) {
        char op; std::string word; std::cin >> op >> word;
        if (op == '+') trie.insert(word);
        else if (op == '-') trie.erase(word);
        else if (op == '?') std::cout << (trie.contains(word) ? "YES" : "NO") << '\n';
    }
}
