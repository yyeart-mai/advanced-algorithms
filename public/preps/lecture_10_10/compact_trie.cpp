#include <iostream>
#include <deque>
#include <memory>
#include <string>
#include <string_view>
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

// Рёбра подписаны непустыми строками; у одного узла первые символы рёбер различны.
template <class CharT>
class CompactTrie {
    using String = std::basic_string<CharT>;
    using View = std::basic_string_view<CharT>;
    struct Node;
    struct Edge { String label; std::unique_ptr<Node> child; };
    struct Node { bool terminal = false; std::vector<Edge> edges; };
    Node root;
    static std::size_t common(View a, View b) {
        std::size_t i = 0;
        while (i < a.size() && i < b.size() && a[i] == b[i]) ++i;
        return i;
    }
    static void insert(Node& node, View word) {
        if (word.empty()) { node.terminal = true; return; }
        for (auto& edge : node.edges) {
            if (edge.label[0] != word[0]) continue;
            std::size_t k = common(edge.label, word);
            if (k == edge.label.size()) { insert(*edge.child, word.substr(k)); return; }
            auto middle = std::make_unique<Node>();
            middle->edges.push_back({edge.label.substr(k), std::move(edge.child)});
            if (k == word.size()) middle->terminal = true;
            else { auto leaf = std::make_unique<Node>(); leaf->terminal = true;
                middle->edges.push_back({String(word.substr(k)), std::move(leaf)}); }
            edge.label.resize(k); edge.child = std::move(middle); return;
        }
        auto leaf = std::make_unique<Node>(); leaf->terminal = true;
        node.edges.push_back({String(word), std::move(leaf)});
    }
    static bool contains(const Node& node, View word) {
        if (word.empty()) return node.terminal;
        for (const auto& edge : node.edges)
            if (edge.label[0] == word[0]) {
                return word.size() >= edge.label.size() &&
                       word.substr(0, edge.label.size()) == edge.label &&
                       contains(*edge.child, word.substr(edge.label.size()));
            }
        return false;
    }
    static bool erase(Node& node, View word) {
        if (word.empty()) {
            if (!node.terminal) return false;
            node.terminal = false;
            return true;
        }
        for (std::size_t i = 0; i < node.edges.size(); ++i) {
            auto& edge = node.edges[i];
            if (word.size() < edge.label.size() ||
                word.substr(0, edge.label.size()) != edge.label) continue;
            if (!erase(*edge.child, word.substr(edge.label.size()))) return false;
            if (!edge.child->terminal && edge.child->edges.empty()) {
                node.edges.erase(node.edges.begin() + i);
            } else if (!edge.child->terminal && edge.child->edges.size() == 1) {
                auto only = std::move(edge.child->edges.front());
                edge.label += only.label;
                edge.child = std::move(only.child);
            }
            return true;
        }
        return false;
    }
public:
    void insert(View word) { insert(root, word); }
    bool contains(View word) const { return contains(root, word); }
    bool erase(View word) { return erase(root, word); }
    template <class InputIt> void insert(InputIt first, InputIt last) { insert(String(first, last)); }
    template <class InputIt> bool contains(InputIt first, InputIt last) const { return contains(String(first, last)); }
    template <class InputIt> bool erase(InputIt first, InputIt last) { return erase(String(first, last)); }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    SimpleCompactTrie trie;
    while (q--) {
        char op; std::string word; std::cin >> op >> word;
        if (op == '+') trie.insert(word);
        else if (op == '-') trie.erase(word);
        else if (op == '?') std::cout << (trie.contains(word) ? "YES" : "NO") << '\n';
    }
}
