#include "patricia.hpp"

#include <iostream>
#include <new>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Patricia dict;
    std::string cmd;

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string key;
                uint64_t value;
                if (!(std::cin >> key >> value)) {
                    break;
                }
                std::cout << (dict.insert(key, value) ? "OK" : "Exist") << '\n';
            } else if (cmd == "-") {
                std::string key;
                if (!(std::cin >> key)) {
                    break;
                }
                std::cout << (dict.erase(key) ? "OK" : "NoSuchWord") << '\n';
            } else if (cmd == "!") {
                std::string action;
                std::string path;
                if (!(std::cin >> action) || !std::getline(std::cin, path)) {
                    break;
                }
                path.erase(0, path.find_first_not_of(" \t"));
                if (!path.empty() && path.back() == '\r') {
                    path.pop_back();
                }
                const char* error;
                if (path.empty()) {
                    error = "Missing dictionary file path";
                } else if (action == "Save") {
                    error = dict.save(path);
                } else if (action == "Load") {
                    error = dict.load(path);
                } else {
                    error = "Unknown file command";
                }
                if (error) {
                    std::cout << "ERROR: " << error << '\n';
                } else {
                    std::cout << "OK\n";
                }
            } else {
                uint64_t value;
                if (dict.find(cmd, value)) {
                    std::cout << "OK: " << value << '\n';
                } else {
                    std::cout << "NoSuchWord\n";
                }
            }
#ifdef PATRICIA_CHECK_INVARIANTS
            if (cmd == "+" || cmd == "-" || cmd == "!") {
                dict.assertInvariants();
            }
#endif
        } catch (const std::bad_alloc&) {
            std::cout << "ERROR: Not enough memory\n";
        }
    }
}
