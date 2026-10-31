#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
    CompactTrie<char> trie;
    while (q--) {
        char op; std::string word; std::cin >> op >> word;
        if (op == '+') trie.insert(word);
        else if (op == '-') trie.erase(word);
        else if (op == '?') std::cout << (trie.contains(word) ? "YES" : "NO") << '\n';
    }
}
