#include <iostream>
#include <map>
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

int main() {
    int q;
    if (!(std::cin >> q)) return 0;
    SimpleTrie trie;
    while (q--) {
        char op;
        std::string word;
        std::cin >> op >> word;
        if (op == '+') trie.insert(word);
        if (op == '-') trie.erase(word);
        if (op == '?') std::cout << (trie.contains(word) ? "YES" : "NO") << '\n';
    }
}
