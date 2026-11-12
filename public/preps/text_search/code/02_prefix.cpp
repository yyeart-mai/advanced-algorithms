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

int main() {
    std::string s;
    std::getline(std::cin, s);
    auto pi = prefixFunction(s);
    for (int i = 0; i < static_cast<int>(pi.size()); ++i) {
        if (i > 0) std::cout << ' ';
        std::cout << pi[i];
    }
    std::cout << '\n';
}
