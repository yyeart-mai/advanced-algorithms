#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> zFunction(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> z(n);
    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) { l = i; r = i + z[i]; }
    }
    return z;
}

int main() {
    std::string s;
    std::getline(std::cin, s);
    auto z = zFunction(s);
    for (std::size_t i = 0; i < z.size(); ++i) std::cout << z[i] << (i + 1 == z.size() ? '\n' : ' ');
}
