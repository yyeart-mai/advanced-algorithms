#include <iostream>
#include <iterator>
#include <string>
#include <vector>

std::vector<int> prefixFunction(const std::string& s) {
    std::vector<int> pi(s.size());
    for (std::size_t i = 1; i < s.size(); ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

// Произвольные сравнимые символы и входные итераторы; вывод индексов через OutputIt.
template <class PatternIt, class TextIt, class OutputIt>
void kmpGeneric(PatternIt pFirst, PatternIt pLast, TextIt tFirst, TextIt tLast, OutputIt out) {
    using Symbol = typename std::iterator_traits<PatternIt>::value_type;
    std::vector<Symbol> pattern(pFirst, pLast);
    if (pattern.empty()) return;
    std::vector<std::size_t> pi(pattern.size());
    for (std::size_t i = 1; i < pattern.size(); ++i) {
        std::size_t j = pi[i - 1];
        while (j && !(pattern[i] == pattern[j])) j = pi[j - 1];
        if (pattern[i] == pattern[j]) ++j;
        pi[i] = j;
    }
    std::size_t j = 0, position = 0;
    for (auto it = tFirst; it != tLast; ++it, ++position) {
        while (j && !(*it == pattern[j])) j = pi[j - 1];
        if (*it == pattern[j]) ++j;
        if (j == pattern.size()) { *out++ = position + 1 - j; j = pi[j - 1]; }
    }
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    if (pattern.empty()) return 0; // пример определён для непустого образца
    auto pi = prefixFunction(pattern);
    int j = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j]) j = pi[j - 1];
        if (text[i] == pattern[j]) ++j;
        if (j == static_cast<int>(pattern.size())) {
            std::cout << i + 1 - pattern.size() << '\n';
            j = pi[j - 1];
        }
    }
}
