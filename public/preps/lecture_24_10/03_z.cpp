#include <algorithm>
#include <iostream>
#include <iterator>
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

// Z-функция для диапазона сравнимых элементов любого типа.
template <class InputIt>
std::vector<std::size_t> zFunctionGeneric(InputIt first, InputIt last) {
    using Symbol = typename std::iterator_traits<InputIt>::value_type;
    std::vector<Symbol> s(first, last);
    std::vector<std::size_t> z(s.size());
    std::size_t l = 0, r = 0;
    for (std::size_t i = 1; i < s.size(); ++i) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < s.size() && s[z[i]] == s[i + z[i]]) ++z[i];
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
