#include <iostream>
#include <string>
#include <vector>

#include "ukkonen_tree.hpp"

int main() {
    std::string text, pattern;
    std::getline(std::cin, text);
    std::getline(std::cin, pattern);
    UkkonenTree tree = byteSuffixTree(text);
    std::vector<int> encoded;
    for (unsigned char symbol : pattern) encoded.push_back(symbol);
    for (std::size_t position : tree.findAll(encoded)) std::cout << position << '\n';
}
