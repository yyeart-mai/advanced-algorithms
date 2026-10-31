#include <iostream>
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
