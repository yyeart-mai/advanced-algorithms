#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "ukkonen_tree.hpp"

struct LcsLocation {
    std::size_t position = 0;
    std::size_t length = 0;
};

// Листы слева от первого разделителя принадлежат первой строке.
// Листы между разделителями принадлежат второй строке.
LcsLocation longestCommonLocation(const UkkonenTree& tree,
                                  std::size_t firstLength,
                                  std::size_t secondLength) {
    std::vector<int> order{0};
    for (std::size_t i = 0; i < order.size(); ++i)
        for (const auto& edge : tree.node(order[i]).next)
            order.push_back(edge.second);

    std::vector<int> owner(tree.nodeCount());
    std::vector<std::size_t> firstPosition(
        tree.nodeCount(), std::numeric_limits<std::size_t>::max());
    LcsLocation best;
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        int node = *it;
        const auto& current = tree.node(node);
        if (current.next.empty()) {
            std::size_t start = current.minSuffix;
            if (start < firstLength) {
                owner[node] = 1;
                firstPosition[node] = start;
            } else if (start > firstLength &&
                       start < firstLength + 1 + secondLength) {
                owner[node] = 2;
            }
        } else {
            for (const auto& edge : current.next) {
                int child = edge.second;
                owner[node] |= owner[child];
                firstPosition[node] = std::min(firstPosition[node],
                                               firstPosition[child]);
            }
        }
        std::size_t depth = static_cast<std::size_t>(current.depth);
        if (owner[node] == 3 &&
            (depth > best.length ||
             (depth == best.length && firstPosition[node] < best.position))) {
            best = {firstPosition[node], depth};
        }
    }
    return best;
}

// Простая версия использует два разных терминатора 256 и 257.
class GeneralizedSuffixTreeSimple {
    std::string first, second;
    UkkonenTree tree;

    static std::vector<int> combine(const std::string& a, const std::string& b) {
        std::vector<int> symbols;
        symbols.reserve(a.size() + b.size() + 2);
        for (unsigned char symbol : a) symbols.push_back(symbol);
        symbols.push_back(256);
        for (unsigned char symbol : b) symbols.push_back(symbol);
        symbols.push_back(257);
        return symbols;
    }
public:
    GeneralizedSuffixTreeSimple(std::string a, std::string b)
        : first(std::move(a)), second(std::move(b)), tree(combine(first, second)) {}

    std::string longestCommonSubstring() const {
        auto place = longestCommonLocation(tree, first.size(), second.size());
        return first.substr(place.position, place.length);
    }
};

// Обобщённая версия кодирует символы обоих диапазонов одинаковыми числами.
template <class InputIt1, class InputIt2,
          class Symbol = typename std::iterator_traits<InputIt1>::value_type,
          class Compare = std::less<Symbol>>
std::vector<Symbol> longestCommonSubstringGeneric(
    InputIt1 aFirst, InputIt1 aLast, InputIt2 bFirst, InputIt2 bLast,
    Compare compare = {}) {
    std::vector<Symbol> a(aFirst, aLast), b(bFirst, bLast);
    std::map<Symbol, int, Compare> codes(compare);
    auto encode = [&](const Symbol& symbol) {
        auto it = codes.find(symbol);
        if (it == codes.end()) {
            int code = static_cast<int>(codes.size());
            it = codes.emplace(symbol, code).first;
        }
        return it->second;
    };
    std::vector<int> encodedA, encodedB;
    for (const Symbol& symbol : a) encodedA.push_back(encode(symbol));
    for (const Symbol& symbol : b) encodedB.push_back(encode(symbol));

    std::vector<int> combined;
    combined.reserve(a.size() + b.size() + 2);
    combined.insert(combined.end(), encodedA.begin(), encodedA.end());
    combined.push_back(static_cast<int>(codes.size()));
    combined.insert(combined.end(), encodedB.begin(), encodedB.end());
    combined.push_back(static_cast<int>(codes.size()) + 1);
    UkkonenTree tree(std::move(combined));
    auto place = longestCommonLocation(tree, a.size(), b.size());
    return {a.begin() + place.position,
            a.begin() + place.position + place.length};
}

int main() {
    std::string first, second;
    std::getline(std::cin, first);
    std::getline(std::cin, second);
    GeneralizedSuffixTreeSimple tree(first, second);
    std::string answer = tree.longestCommonSubstring();
    std::cout << answer.size() << '\n' << answer << '\n';
}
