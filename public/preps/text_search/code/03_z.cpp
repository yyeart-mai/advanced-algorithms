#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Число 256 позволяет отделить образец от текста без выбора особого байта.
std::vector<int> zFunction(const std::vector<int>& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> z(n);
    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }
    return z;
}

std::vector<int> zSearch(const std::string& pattern, const std::string& text) {
    std::vector<int> answer;
    if (pattern.empty()) return answer;
    std::vector<int> joined;
    for (unsigned char c : pattern) joined.push_back(c);
    joined.push_back(256);
    for (unsigned char c : text) joined.push_back(c);
    auto z = zFunction(joined);
    int m = static_cast<int>(pattern.size());
    for (int i = m + 1; i < static_cast<int>(joined.size()); ++i)
        if (z[i] == m) answer.push_back(i - m - 1);
    return answer;
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    for (int start : zSearch(pattern, text)) std::cout << start << '\n';
}
