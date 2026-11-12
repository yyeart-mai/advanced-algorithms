#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

// Таблица хранит следующее состояние для каждого байта и каждой длины
// совпавшего префикса. После построения на байт текста нужен один переход.
class RealTimeKmpSimple {
    std::vector<std::array<std::size_t, 256>> next;
    std::size_t matched = 0;
    std::size_t length = 0;
public:
    explicit RealTimeKmpSimple(const std::string& pattern)
        : next(pattern.size() + 1), length(pattern.size()) {
        if (pattern.empty()) return;

        std::vector<std::size_t> pi(length);
        for (std::size_t i = 1; i < length; ++i) {
            std::size_t j = pi[i - 1];
            while (j > 0 && pattern[i] != pattern[j]) j = pi[j - 1];
            if (pattern[i] == pattern[j]) ++j;
            pi[i] = j;
        }

        next[0][static_cast<unsigned char>(pattern[0])] = 1;
        for (std::size_t state = 1; state <= length; ++state) {
            next[state] = next[pi[state - 1]];
            if (state < length)
                next[state][static_cast<unsigned char>(pattern[state])] = state + 1;
        }
    }

    bool feed(char symbol) {
        if (length == 0) return false;
        matched = next[matched][static_cast<unsigned char>(symbol)];
        return matched == length;
    }
};

// Обобщённая версия. index(symbol) должен вернуть число от 0 до alphabetSize-1.
// Если index работает за O(1), то feed тоже работает за O(1).
template <class Symbol, class Index>
class RealTimeKmpGeneric {
    std::vector<std::vector<std::size_t>> next;
    Index index;
    std::size_t matched = 0;
    std::size_t length = 0;
public:
    template <class PatternIt>
    RealTimeKmpGeneric(PatternIt first, PatternIt last,
                       std::size_t alphabetSize, Index symbolIndex)
        : index(symbolIndex) {
        std::vector<Symbol> pattern(first, last);
        length = pattern.size();
        next.assign(length + 1, std::vector<std::size_t>(alphabetSize));
        if (pattern.empty()) return;

        std::vector<std::size_t> pi(length);
        for (std::size_t i = 1; i < length; ++i) {
            std::size_t j = pi[i - 1];
            while (j > 0 && !(pattern[i] == pattern[j])) j = pi[j - 1];
            if (pattern[i] == pattern[j]) ++j;
            pi[i] = j;
        }

        next[0][index(pattern[0])] = 1;
        for (std::size_t state = 1; state <= length; ++state) {
            next[state] = next[pi[state - 1]];
            if (state < length) next[state][index(pattern[state])] = state + 1;
        }
    }

    bool feed(const Symbol& symbol) {
        if (length == 0) return false;
        matched = next[matched][index(symbol)];
        return matched == length;
    }
};

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    if (pattern.empty()) return 0;

    RealTimeKmpSimple matcher(pattern);
    for (std::size_t i = 0; i < text.size(); ++i)
        if (matcher.feed(text[i])) std::cout << i + 1 - pattern.size() << '\n';
}
