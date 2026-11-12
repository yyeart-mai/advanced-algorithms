#include <iostream>
#include <deque>
#include <string>
#include <utility>
#include <vector>

// Простая версия для std::string. Каждое ребро хранит сразу несколько букв.
class SimpleCompactTrie {
    struct Edge { std::string label; int child; };
    struct Node { bool terminal = false; std::vector<Edge> edges; };
    std::deque<Node> nodes = std::deque<Node>(1); // Узлы не сдвигаются при добавлении.

    void insert(int p, const std::string& word) {
        if (word.empty()) { nodes[p].terminal = true; return; }
        for (Edge& edge : nodes[p].edges) {
            if (edge.label[0] != word[0]) continue;
            int common = 0;
            while (common < static_cast<int>(edge.label.size()) &&
                   common < static_cast<int>(word.size()) &&
                   edge.label[common] == word[common]) ++common;
            if (common == static_cast<int>(edge.label.size())) {
                insert(edge.child, word.substr(common));
                return;
            }
            int middle = static_cast<int>(nodes.size());
            nodes.emplace_back();
            nodes[middle].edges.push_back({edge.label.substr(common), edge.child});
            if (common == static_cast<int>(word.size())) nodes[middle].terminal = true;
            else {
                int leaf = static_cast<int>(nodes.size());
                nodes.emplace_back();
                nodes[leaf].terminal = true;
                nodes[middle].edges.push_back({word.substr(common), leaf});
            }
            edge.label.resize(common);
            edge.child = middle;
            return;
        }
        int leaf = static_cast<int>(nodes.size());
        nodes.emplace_back();
        nodes[leaf].terminal = true;
        nodes[p].edges.push_back({word, leaf});
    }
    bool contains(int p, const std::string& word) const {
        if (word.empty()) return nodes[p].terminal;
        for (const Edge& edge : nodes[p].edges) {
            if (word.compare(0, edge.label.size(), edge.label) == 0)
                return contains(edge.child, word.substr(edge.label.size()));
        }
        return false;
    }
    bool erase(int p, const std::string& word) {
        if (word.empty()) {
            if (!nodes[p].terminal) return false;
            nodes[p].terminal = false;
            return true;
        }
        for (int i = 0; i < static_cast<int>(nodes[p].edges.size()); ++i) {
            Edge& edge = nodes[p].edges[i];
            if (word.compare(0, edge.label.size(), edge.label) != 0) continue;
            if (!erase(edge.child, word.substr(edge.label.size()))) return false;
            Node& child = nodes[edge.child];
            if (!child.terminal && child.edges.empty()) {
                nodes[p].edges.erase(nodes[p].edges.begin() + i);
            } else if (!child.terminal && child.edges.size() == 1) {
                edge.label += child.edges[0].label;
                edge.child = child.edges[0].child;
            }
            return true;
        }
        return false;
    }
public:
    void insert(const std::string& word) { insert(0, word); }
    bool contains(const std::string& word) const { return contains(0, word); }
    bool erase(const std::string& word) { return erase(0, word); }
};

int main() {
    int q;
    if (!(std::cin >> q)) return 0;
    SimpleCompactTrie trie;
    while (q--) {
        char op;
        std::string word;
        std::cin >> op >> word;
        if (op == '+') trie.insert(word);
        if (op == '-') trie.erase(word);
        if (op == '?') std::cout << (trie.contains(word) ? "YES" : "NO") << '\n';
    }
}
