#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "ukkonen_tree.hpp"

// Для каждого суффикса запроса находим его самый длинный префикс в тексте.
// Результат содержит длину и самую раннюю позицию в тексте.
std::vector<std::pair<std::size_t, std::size_t>> matchingStatisticsSimple(
    const std::string& reference, const std::string& query) {
    UkkonenTree tree = byteSuffixTree(reference);
    std::vector<int> encoded;
    for (unsigned char symbol : query) encoded.push_back(symbol);
    std::vector<std::pair<std::size_t, std::size_t>> result;
    for (std::size_t i = 0; i < encoded.size(); ++i)
        result.push_back(tree.longestPrefix(encoded, i));
    return result;
}

// Шаблонная версия принимает диапазоны произвольных сравнимых символов.
template <class ReferenceIt, class QueryIt,
          class Symbol = typename std::iterator_traits<ReferenceIt>::value_type,
          class Compare = std::less<Symbol>>
std::vector<std::pair<std::size_t, std::size_t>> matchingStatisticsGeneric(
    ReferenceIt referenceFirst, ReferenceIt referenceLast,
    QueryIt queryFirst, QueryIt queryLast, Compare compare = {}) {
    UkkonenIndex<Symbol, Compare> index(referenceFirst, referenceLast, compare);
    std::vector<Symbol> query(queryFirst, queryLast);
    std::vector<std::pair<std::size_t, std::size_t>> result;
    for (std::size_t i = 0; i < query.size(); ++i)
        result.push_back(index.longestPrefix(query.begin() + i, query.end()));
    return result;
}

int main() {
    std::string reference, query;
    std::getline(std::cin, reference);
    std::getline(std::cin, query);
    for (auto [length, position] : matchingStatisticsSimple(reference, query)) {
        if (length == 0) std::cout << "0 -1\n";
        else std::cout << length << ' ' << position << '\n';
    }
}
