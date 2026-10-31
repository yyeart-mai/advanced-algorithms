#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Для любых сравнимых символов; выдаёт индексы всех вхождений, включая перекрывающиеся.
template <class PatternIt, class TextIt, class OutputIt>
void naiveSearch(PatternIt pFirst, PatternIt pLast, TextIt tFirst, TextIt tLast,
                 OutputIt out) {
    using Symbol = typename std::iterator_traits<PatternIt>::value_type;
    std::vector<Symbol> pattern(pFirst, pLast);
    if (pattern.empty()) return; // В этом примере пустой образец не ищем.
    std::size_t position = 0;
    for (auto start = tFirst; start != tLast; ++start, ++position) {
        auto text = start;
        std::size_t i = 0;
        while (text != tLast && i < pattern.size() && *text == pattern[i]) { ++text; ++i; }
        if (i == pattern.size()) *out++ = position;
    }
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    std::vector<std::size_t> positions;
    naiveSearch(pattern.begin(), pattern.end(), text.begin(), text.end(),
                std::back_inserter(positions));
    for (auto p : positions) std::cout << p << '\n';
}
