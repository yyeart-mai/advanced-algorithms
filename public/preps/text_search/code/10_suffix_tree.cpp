#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

// Простая версия для байтовой строки. Значение 256 служит терминатором.
// Метка ребра хранится как полуинтервал [begin, end) в исходном тексте.
class SuffixTreeSimple {
    struct Edge { std::size_t begin, end, child; };
    struct Node {
        std::map<int, Edge> edges;
        std::size_t suffix = std::numeric_limits<std::size_t>::max();
    };
    std::vector<int> symbols;
    std::vector<Node> nodes = std::vector<Node>(1);

    void insertSuffix(std::size_t start) {
        std::size_t node = 0, position = start;
        while (position < symbols.size()) {
            auto it = nodes[node].edges.find(symbols[position]);
            if (it == nodes[node].edges.end()) {
                std::size_t leaf = nodes.size();
                nodes.emplace_back();
                nodes[leaf].suffix = start;
                nodes[node].edges[symbols[position]] = {position, symbols.size(), leaf};
                return;
            }

            Edge edge = it->second;
            std::size_t common = 0;
            while (position + common < symbols.size() &&
                   edge.begin + common < edge.end &&
                   symbols[position + common] == symbols[edge.begin + common])
                ++common;
            if (edge.begin + common == edge.end) {
                node = edge.child;
                position += common;
                continue;
            }

            std::size_t middle = nodes.size();
            nodes.emplace_back();
            std::size_t leaf = nodes.size();
            nodes.emplace_back();
            nodes[leaf].suffix = start;
            nodes[middle].edges[symbols[edge.begin + common]] =
                {edge.begin + common, edge.end, edge.child};
            nodes[middle].edges[symbols[position + common]] =
                {position + common, symbols.size(), leaf};
            nodes[node].edges[symbols[position]] =
                {edge.begin, edge.begin + common, middle};
            return;
        }
    }
public:
    explicit SuffixTreeSimple(const std::string& text) {
        for (unsigned char symbol : text) symbols.push_back(symbol);
        symbols.push_back(256);
        for (std::size_t start = 0; start < symbols.size(); ++start)
            insertSuffix(start);
    }

    std::vector<std::size_t> find(const std::string& pattern) const {
        std::vector<std::size_t> result;
        if (pattern.empty()) return result;
        std::size_t node = 0, position = 0;
        while (position < pattern.size()) {
            int symbol = static_cast<unsigned char>(pattern[position]);
            auto it = nodes[node].edges.find(symbol);
            if (it == nodes[node].edges.end()) return {};
            Edge edge = it->second;
            for (std::size_t i = edge.begin; i < edge.end && position < pattern.size();
                 ++i, ++position) {
                if (symbols[i] != static_cast<unsigned char>(pattern[position])) return {};
            }
            node = edge.child;
        }

        std::vector<std::size_t> stack{node};
        while (!stack.empty()) {
            std::size_t current = stack.back();
            stack.pop_back();
            if (nodes[current].suffix != std::numeric_limits<std::size_t>::max())
                result.push_back(nodes[current].suffix);
            for (const auto& entry : nodes[current].edges)
                stack.push_back(entry.second.child);
        }
        std::sort(result.begin(), result.end());
        return result;
    }
};

int main() {
    std::string text, pattern;
    std::getline(std::cin, text);
    std::getline(std::cin, pattern);
    SuffixTreeSimple tree(text);
    for (std::size_t start : tree.find(pattern)) std::cout << start << '\n';
}
