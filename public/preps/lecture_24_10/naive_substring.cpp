#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Простая версия для строк. Пустой образец в этом примере не ищем.
std::vector<std::size_t> naiveSearchSimple(const std::string& pattern,
                                           const std::string& text) {
    std::vector<std::size_t> positions;
    if (pattern.empty()) return positions;
    for (std::size_t start = 0; start + pattern.size() <= text.size(); ++start) {
        std::size_t i = 0;
        while (i < pattern.size() && text[start + i] == pattern[i]) ++i;
        if (i == pattern.size()) positions.push_back(start);
    }
    return positions;
}

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
    auto positions = naiveSearchSimple(pattern, text);
    for (auto p : positions) std::cout << p << '\n';
}
