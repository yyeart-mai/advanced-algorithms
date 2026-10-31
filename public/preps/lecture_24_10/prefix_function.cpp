#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// pi[i] — длина наибольшего собственного префикса отрезка [0, i],
// совпадающего с его суффиксом. Пример дополнительный: название функции
// препроцессинга в конспекте лекции не уточнено.
template <class InputIt>
std::vector<std::size_t> prefixFunction(InputIt first, InputIt last) {
    using Symbol = typename std::iterator_traits<InputIt>::value_type;
    std::vector<Symbol> s(first, last);
    std::vector<std::size_t> pi(s.size());
    for (std::size_t i = 1; i < s.size(); ++i) {
        std::size_t j = pi[i - 1];
        while (j && !(s[i] == s[j])) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

int main() {
    std::string s; std::getline(std::cin, s);
    auto pi = prefixFunction(s.begin(), s.end());
    for (std::size_t i = 0; i < pi.size(); ++i)
        std::cout << pi[i] << (i + 1 == pi.size() ? '\n' : ' ');
}
