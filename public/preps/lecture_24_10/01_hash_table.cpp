#include <iostream>
#include <functional>
#include <string>
#include <utility>
#include <vector>

// Учебное множество строк: отдельные цепочки, без удаления.
class HashSet {
    std::vector<std::vector<std::string>> buckets;
    std::size_t count = 0;
    static std::size_t hash(const std::string& s) {
        std::size_t h = 0;
        for (unsigned char c : s) h = h * 131 + c;
        return h;
    }
    void grow() {
        auto old = std::move(buckets);
        buckets.resize(old.size() * 2 + 1);
        for (auto& chain : old)
            for (auto& s : chain) buckets[hash(s) % buckets.size()].push_back(std::move(s));
    }
public:
    HashSet() : buckets(7) {}
    bool contains(const std::string& s) const {
        for (const auto& x : buckets[hash(s) % buckets.size()]) if (x == s) return true;
        return false;
    }
    void insert(const std::string& s) {
        if (contains(s)) return;
        if (count + 1 > buckets.size()) grow();
        buckets[hash(s) % buckets.size()].push_back(s);
        ++count;
    }
};

// Произвольный ключ, пользовательские хеш и сравнение; вставка диапазона итераторов.
template <class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class HashSetGeneric {
    std::vector<std::vector<Key>> buckets{7};
    std::size_t count = 0;
    Hash hash;
    Equal equal;
    void grow() {
        auto old = std::move(buckets);
        buckets.resize(old.size() * 2 + 1);
        for (auto& chain : old)
            for (auto& x : chain) buckets[hash(x) % buckets.size()].push_back(std::move(x));
    }
public:
    bool contains(const Key& x) const {
        for (const auto& item : buckets[hash(x) % buckets.size()]) if (equal(item, x)) return true;
        return false;
    }
    void insert(const Key& x) {
        if (contains(x)) return;
        if (count + 1 > buckets.size()) grow();
        buckets[hash(x) % buckets.size()].push_back(x);
        ++count;
    }
    template <class InputIt>
    void insert(InputIt first, InputIt last) {
        for (; first != last; ++first) insert(*first);
    }
};

int main() {
    int q;
    if (!(std::cin >> q)) return 0;
    HashSet set;
    while (q--) {
        char op; std::string s;
        std::cin >> op >> s;
        if (op == '+') set.insert(s);
        else if (op == '?') std::cout << (set.contains(s) ? "YES" : "NO") << '\n';
    }
}
