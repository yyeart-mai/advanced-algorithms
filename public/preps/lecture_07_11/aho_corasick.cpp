#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Простая версия для строк из байтов. Пустые образцы игнорируются.
class AhoCorasickSimple {
    struct Node {
        std::array<int, 256> next;
        int link = 0;
        std::vector<int> output;
        Node() { next.fill(-1); }
    };
    std::vector<Node> nodes = std::vector<Node>(1);
    std::vector<std::size_t> lengths;

    void addPattern(const std::string& pattern, int id) {
        if (pattern.empty()) return;
        int state = 0;
        for (unsigned char symbol : pattern) {
            if (nodes[state].next[symbol] == -1) {
                nodes[state].next[symbol] = static_cast<int>(nodes.size());
                nodes.emplace_back();
            }
            state = nodes[state].next[symbol];
        }
        nodes[state].output.push_back(id);
    }

    void build() {
        std::queue<int> queue;
        for (int symbol = 0; symbol < 256; ++symbol) {
            int child = nodes[0].next[symbol];
            if (child == -1) nodes[0].next[symbol] = 0;
            else queue.push(child);
        }
        while (!queue.empty()) {
            int state = queue.front();
            queue.pop();
            int link = nodes[state].link;
            const auto& suffixOutput = nodes[link].output;
            nodes[state].output.insert(nodes[state].output.end(),
                                       suffixOutput.begin(), suffixOutput.end());
            std::sort(nodes[state].output.begin(), nodes[state].output.end());
            for (int symbol = 0; symbol < 256; ++symbol) {
                int child = nodes[state].next[symbol];
                if (child == -1) nodes[state].next[symbol] = nodes[link].next[symbol];
                else {
                    nodes[child].link = nodes[link].next[symbol];
                    queue.push(child);
                }
            }
        }
    }
public:
    explicit AhoCorasickSimple(const std::vector<std::string>& patterns) {
        for (std::size_t id = 0; id < patterns.size(); ++id) {
            lengths.push_back(patterns[id].size());
            addPattern(patterns[id], static_cast<int>(id));
        }
        build();
    }

    std::vector<std::pair<std::size_t, int>> search(const std::string& text) const {
        std::vector<std::pair<std::size_t, int>> result;
        int state = 0;
        for (std::size_t i = 0; i < text.size(); ++i) {
            state = nodes[state].next[static_cast<unsigned char>(text[i])];
            for (int id : nodes[state].output)
                result.push_back({i + 1 - lengths[id], id});
        }
        return result;
    }
};

// Обобщённая версия принимает последовательности произвольных сравнимых символов.
// Образцы добавляются до build, затем search читает текст через входные итераторы.
template <class Symbol, class Compare = std::less<Symbol>>
class AhoCorasickGeneric {
    struct Node {
        std::map<Symbol, int, Compare> next;
        int link = 0;
        std::vector<std::size_t> output;
        explicit Node(Compare compare) : next(compare) {}
    };
    Compare compare;
    std::vector<Node> nodes;
    std::vector<std::size_t> lengths;
    bool ready = false;
public:
    explicit AhoCorasickGeneric(Compare comp = {}) : compare(comp) {
        nodes.emplace_back(compare);
    }

    template <class InputIt>
    std::size_t addPattern(InputIt first, InputIt last) {
        if (ready) throw std::logic_error("add patterns before build");
        std::size_t id = lengths.size();
        std::size_t length = 0;
        int state = 0;
        for (; first != last; ++first) {
            auto edge = nodes[state].next.find(*first);
            if (edge == nodes[state].next.end()) {
                int child = static_cast<int>(nodes.size());
                nodes[state].next.emplace(*first, child);
                nodes.emplace_back(compare);
                state = child;
            } else state = edge->second;
            ++length;
        }
        lengths.push_back(length);
        if (length > 0) nodes[state].output.push_back(id);
        return id;
    }

    void build() {
        if (ready) return;
        std::queue<int> queue;
        for (const auto& edge : nodes[0].next) queue.push(edge.second);
        while (!queue.empty()) {
            int state = queue.front();
            queue.pop();
            int link = nodes[state].link;
            const auto& suffixOutput = nodes[link].output;
            nodes[state].output.insert(nodes[state].output.end(),
                                       suffixOutput.begin(), suffixOutput.end());
            std::sort(nodes[state].output.begin(), nodes[state].output.end());
            for (const auto& edge : nodes[state].next) {
                const Symbol& symbol = edge.first;
                int child = edge.second;
                int fallback = link;
                while (fallback && nodes[fallback].next.find(symbol) == nodes[fallback].next.end())
                    fallback = nodes[fallback].link;
                auto next = nodes[fallback].next.find(symbol);
                nodes[child].link = next == nodes[fallback].next.end() ? 0 : next->second;
                queue.push(child);
            }
        }
        ready = true;
    }

    template <class InputIt, class OutputIt>
    void search(InputIt first, InputIt last, OutputIt out) const {
        if (!ready) throw std::logic_error("call build before search");
        int state = 0;
        std::size_t position = 0;
        for (; first != last; ++first, ++position) {
            while (state && nodes[state].next.find(*first) == nodes[state].next.end())
                state = nodes[state].link;
            auto edge = nodes[state].next.find(*first);
            if (edge != nodes[state].next.end()) state = edge->second;
            for (std::size_t id : nodes[state].output)
                *out++ = std::pair<std::size_t, std::size_t>{position + 1 - lengths[id], id};
        }
    }
};

int main() {
    int count;
    if (!(std::cin >> count) || count < 0) return 0;
    std::string line;
    std::getline(std::cin, line);
    std::vector<std::string> patterns(count);
    for (std::string& pattern : patterns) std::getline(std::cin, pattern);
    std::string text;
    std::getline(std::cin, text);

    AhoCorasickSimple matcher(patterns);
    for (auto [position, id] : matcher.search(text))
        std::cout << position << ' ' << id << '\n';
}
