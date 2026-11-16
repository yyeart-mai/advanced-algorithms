#pragma once

#include <algorithm>
#include <climits>
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Целочисленное дерево. Последний символ входа должен быть уникальным
// терминатором. У листа end == -1 означает общий растущий конец.
class UkkonenTree {
public:
    struct Node {
        std::map<int, int> next;
        int start = -1;
        int end = -1;
        int link = 0;
        int depth = 0;
        std::size_t minSuffix = std::numeric_limits<std::size_t>::max();
        Node(int begin, int finish) : start(begin), end(finish) {}
    };

private:
    std::vector<int> text;
    std::vector<Node> nodes;
    int activeNode = 0;
    int activeEdge = 0;
    int activeLength = 0;
    int remainder = 0;
    int leafEnd = -1;

    int newNode(int start, int end) {
        int id = static_cast<int>(nodes.size());
        nodes.emplace_back(start, end);
        return id;
    }

    int edgeEnd(int node) const {
        return nodes[node].end == -1 ? leafEnd : nodes[node].end;
    }

    void extend(int position) {
        leafEnd = position;
        ++remainder;
        int previousInternal = -1;

        while (remainder > 0) {
            if (activeLength == 0) activeEdge = position;
            int firstSymbol = text[activeEdge];
            auto edge = nodes[activeNode].next.find(firstSymbol);

            if (edge == nodes[activeNode].next.end()) {
                int leaf = newNode(position, -1);
                nodes[activeNode].next[firstSymbol] = leaf;
                if (previousInternal != -1) {
                    nodes[previousInternal].link = activeNode;
                    previousInternal = -1;
                }
            } else {
                int child = edge->second;
                int length = edgeEnd(child) - nodes[child].start + 1;
                if (activeLength >= length) {
                    activeEdge += length;
                    activeLength -= length;
                    activeNode = child;
                    continue;
                }

                int nextSymbol = text[nodes[child].start + activeLength];
                if (nextSymbol == text[position]) {
                    if (previousInternal != -1 && activeNode != 0)
                        nodes[previousInternal].link = activeNode;
                    ++activeLength;
                    break;
                }

                int split = newNode(nodes[child].start,
                                    nodes[child].start + activeLength - 1);
                nodes[activeNode].next[firstSymbol] = split;
                int leaf = newNode(position, -1);
                nodes[split].next[text[position]] = leaf;
                nodes[child].start += activeLength;
                nodes[split].next[text[nodes[child].start]] = child;
                if (previousInternal != -1) nodes[previousInternal].link = split;
                previousInternal = split;
            }

            --remainder;
            if (activeNode == 0 && activeLength > 0) {
                --activeLength;
                activeEdge = position - remainder + 1;
            } else if (activeNode != 0) {
                activeNode = nodes[activeNode].link;
            }
        }
    }

    void annotate() {
        std::vector<int> order{0};
        for (std::size_t i = 0; i < order.size(); ++i) {
            int parent = order[i];
            for (const auto& edge : nodes[parent].next) {
                int child = edge.second;
                nodes[child].depth = nodes[parent].depth + edgeLength(child);
                order.push_back(child);
            }
        }
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int node = *it;
            if (nodes[node].next.empty()) {
                nodes[node].minSuffix = text.size() - nodes[node].depth;
            } else {
                for (const auto& edge : nodes[node].next)
                    nodes[node].minSuffix = std::min(nodes[node].minSuffix,
                                                     nodes[edge.second].minSuffix);
            }
        }
    }

public:
    explicit UkkonenTree(std::vector<int> symbols) : text(std::move(symbols)) {
        if (text.empty() || text.size() > static_cast<std::size_t>(INT_MAX))
            throw std::invalid_argument("text length is outside the supported range");
        for (std::size_t i = 0; i + 1 < text.size(); ++i)
            if (text[i] == text.back())
                throw std::invalid_argument("the final symbol must be unique");
        nodes.emplace_back(-1, -1);
        for (int i = 0; i < static_cast<int>(text.size()); ++i) extend(i);
        annotate();
    }

    const Node& node(int id) const { return nodes[id]; }
    int nodeCount() const { return static_cast<int>(nodes.size()); }
    int edgeLength(int id) const { return edgeEnd(id) - nodes[id].start + 1; }

    // Возвращает длину совпавшего префикса и позицию одного вхождения.
    // Для нулевой длины позиция равна max(size_t).
    std::pair<std::size_t, std::size_t> longestPrefix(
        const std::vector<int>& query, std::size_t offset = 0) const {
        std::size_t matched = 0;
        int state = 0;
        while (offset + matched < query.size()) {
            auto edge = nodes[state].next.find(query[offset + matched]);
            if (edge == nodes[state].next.end()) break;
            int child = edge->second;
            for (int i = nodes[child].start;
                 i <= edgeEnd(child) && offset + matched < query.size(); ++i) {
                if (text[i] != query[offset + matched])
                    return {matched, matched ? nodes[child].minSuffix
                                             : std::numeric_limits<std::size_t>::max()};
                ++matched;
            }
            state = child;
        }
        return {matched, matched ? nodes[state].minSuffix
                                 : std::numeric_limits<std::size_t>::max()};
    }

    std::vector<std::size_t> findAll(const std::vector<int>& pattern) const {
        if (pattern.empty()) return {};
        std::size_t position = 0;
        int state = 0;
        while (position < pattern.size()) {
            auto edge = nodes[state].next.find(pattern[position]);
            if (edge == nodes[state].next.end()) return {};
            int child = edge->second;
            for (int i = nodes[child].start;
                 i <= edgeEnd(child) && position < pattern.size(); ++i, ++position)
                if (text[i] != pattern[position]) return {};
            state = child;
        }
        std::vector<std::size_t> result;
        std::vector<int> stack{state};
        while (!stack.empty()) {
            int current = stack.back();
            stack.pop_back();
            if (nodes[current].next.empty()) result.push_back(nodes[current].minSuffix);
            for (const auto& edge : nodes[current].next) stack.push_back(edge.second);
        }
        std::sort(result.begin(), result.end());
        return result;
    }
};

inline UkkonenTree byteSuffixTree(const std::string& text) {
    std::vector<int> symbols;
    symbols.reserve(text.size() + 1);
    for (unsigned char symbol : text) symbols.push_back(symbol);
    symbols.push_back(256);
    return UkkonenTree(std::move(symbols));
}

// Для другого типа символов коды создаются при чтении диапазона итераторов.
// Значение codes.size() зарезервировано для терминатора.
template <class Symbol, class Compare = std::less<Symbol>>
class UkkonenIndex {
    std::map<Symbol, int, Compare> codes;
    std::unique_ptr<UkkonenTree> tree;
public:
    template <class InputIt>
    UkkonenIndex(InputIt first, InputIt last, Compare compare = {}) : codes(compare) {
        std::vector<int> encoded;
        for (; first != last; ++first) {
            auto it = codes.find(*first);
            if (it == codes.end()) {
                int code = static_cast<int>(codes.size());
                it = codes.emplace(*first, code).first;
            }
            encoded.push_back(it->second);
        }
        encoded.push_back(static_cast<int>(codes.size()));
        tree = std::make_unique<UkkonenTree>(std::move(encoded));
    }

    template <class InputIt, class OutputIt>
    void findAll(InputIt first, InputIt last, OutputIt out) const {
        std::vector<int> pattern;
        for (; first != last; ++first) {
            auto it = codes.find(*first);
            if (it == codes.end()) return;
            pattern.push_back(it->second);
        }
        for (std::size_t position : tree->findAll(pattern)) *out++ = position;
    }

    template <class InputIt>
    std::pair<std::size_t, std::size_t> longestPrefix(InputIt first,
                                                      InputIt last) const {
        std::vector<int> query;
        for (; first != last; ++first) {
            auto it = codes.find(*first);
            if (it == codes.end()) break;
            query.push_back(it->second);
        }
        return tree->longestPrefix(query);
    }
};
