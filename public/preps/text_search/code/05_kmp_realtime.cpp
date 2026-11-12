#include <array>
#include <iostream>
#include <string>
#include <vector>

class RealTimeKmp {
    std::vector<std::array<int, 256>> go;
    int state = 0;
    int m;
public:
    explicit RealTimeKmp(const std::string& pattern)
        : go(pattern.size() + 1), m(static_cast<int>(pattern.size())) {
        if (m == 0) return;
        std::vector<int> pi(m);
        for (int i = 1; i < m; ++i) {
            int j = pi[i - 1];
            while (j > 0 && pattern[i] != pattern[j]) j = pi[j - 1];
            if (pattern[i] == pattern[j]) ++j;
            pi[i] = j;
        }
        go[0].fill(0);
        go[0][static_cast<unsigned char>(pattern[0])] = 1;
        for (int q = 1; q <= m; ++q) {
            go[q] = go[pi[q - 1]];
            if (q < m) go[q][static_cast<unsigned char>(pattern[q])] = q + 1;
        }
    }

    bool feed(char c) {
        if (m == 0) return false;
        state = go[state][static_cast<unsigned char>(c)];
        return state == m;
    }
};

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    RealTimeKmp matcher(pattern);
    for (int i = 0; i < static_cast<int>(text.size()); ++i)
        if (matcher.feed(text[i]))
            std::cout << i - static_cast<int>(pattern.size()) + 1 << '\n';
}
