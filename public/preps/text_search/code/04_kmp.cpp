#include <iostream>
#include <string>
#include <vector>

std::vector<int> prefixFunction(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> pi(n);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

std::vector<int> kmpSearch(const std::string& pattern, const std::string& text) {
    std::vector<int> answer;
    int m = static_cast<int>(pattern.size());
    if (m == 0) return answer;
    auto pi = prefixFunction(pattern);
    int j = 0;
    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        while (j > 0 && text[i] != pattern[j]) j = pi[j - 1];
        if (text[i] == pattern[j]) ++j;
        if (j == m) {
            answer.push_back(i - m + 1);
            j = pi[m - 1];
        }
    }
    return answer;
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    for (int start : kmpSearch(pattern, text)) std::cout << start << '\n';
}
