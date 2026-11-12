#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

// shift[j] задаёт сдвиг после несовпадения в позиции j-1.
// shift[0] задаёт сдвиг после полного совпадения.
std::vector<std::size_t> goodSuffixSimple(const std::string& pattern) {
    std::size_t m = pattern.size();
    std::vector<std::size_t> shift(m + 1), border(m + 1);
    std::size_t i = m, j = m + 1;
    border[i] = j;

    while (i > 0) {
        while (j <= m && pattern[i - 1] != pattern[j - 1]) {
            if (shift[j] == 0) shift[j] = j - i;
            j = border[j];
        }
        --i;
        --j;
        border[i] = j;
    }

    j = border[0];
    for (i = 0; i <= m; ++i) {
        if (shift[i] == 0) shift[i] = j;
        if (i == j) j = border[j];
    }
    return shift;
}

std::vector<std::size_t> boyerMooreSimple(const std::string& pattern,
                                          const std::string& text) {
    std::vector<std::size_t> positions;
    if (pattern.empty() || pattern.size() > text.size()) return positions;
    std::size_t m = pattern.size();

    std::array<std::ptrdiff_t, 256> last;
    last.fill(-1);
    for (std::size_t i = 0; i < m; ++i)
        last[static_cast<unsigned char>(pattern[i])] = static_cast<std::ptrdiff_t>(i);
    auto shift = goodSuffixSimple(pattern);

    std::size_t start = 0;
    while (start <= text.size() - m) {
        std::ptrdiff_t j = static_cast<std::ptrdiff_t>(m) - 1;
        while (j >= 0 && pattern[j] == text[start + j]) --j;
        if (j < 0) {
            positions.push_back(start);
            start += shift[0];
        } else {
            std::ptrdiff_t bad = j - last[static_cast<unsigned char>(text[start + j])];
            std::size_t badShift = bad > 0 ? static_cast<std::size_t>(bad) : 1;
            start += std::max(badShift, shift[j + 1]);
        }
    }
    return positions;
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    for (std::size_t start : boyerMooreSimple(pattern, text))
        std::cout << start << '\n';
}
