#include <iostream>
#include <string>
#include <vector>

std::vector<int> naiveSearch(const std::string& pattern, const std::string& text) {
    int m = static_cast<int>(pattern.size());
    int n = static_cast<int>(text.size());
    std::vector<int> answer;
    if (m == 0) return answer;
    for (int start = 0; start <= n - m; ++start) {
        int j = 0;
        while (j < m && pattern[j] == text[start + j]) ++j;
        if (j == m) answer.push_back(start);
    }
    return answer;
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    for (int start : naiveSearch(pattern, text)) std::cout << start << '\n';
}
