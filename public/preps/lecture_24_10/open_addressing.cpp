#include <functional>
#include <iostream>
#include <optional>
#include <vector>

// Линейное пробирование; tombstone нужен, чтобы удаление не обрывало цепочку поиска.
template <class Key, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class OpenAddressSet {
    struct Slot { std::optional<Key> value; bool deleted = false; };
    std::vector<Slot> slots = std::vector<Slot>(8);
    std::size_t used = 0, occupied = 0;
    Hash hash;
    Equal equal;
    std::size_t locate(const Key& x) const {
        std::size_t i = hash(x) % slots.size();
        while (slots[i].value || slots[i].deleted) {
            if (slots[i].value && equal(*slots[i].value, x)) return i;
            i = (i + 1) % slots.size();
        }
        return slots.size();
    }
    void grow() {
        auto old = std::move(slots);
        slots = std::vector<Slot>(old.size() * 2);
        used = occupied = 0;
        for (const auto& slot : old) if (slot.value) insert(*slot.value);
    }
public:
    bool contains(const Key& x) const { return locate(x) != slots.size(); }
    void insert(const Key& x) {
        if (contains(x)) return;
        if ((occupied + 1) * 2 >= slots.size()) grow();
        std::size_t i = hash(x) % slots.size(), firstDeleted = slots.size();
        while (slots[i].value || slots[i].deleted) {
            if (!slots[i].value && firstDeleted == slots.size()) firstDeleted = i;
            i = (i + 1) % slots.size();
        }
        if (firstDeleted != slots.size()) i = firstDeleted;
        else ++occupied;
        slots[i].value = x; slots[i].deleted = false; ++used;
    }
    bool erase(const Key& x) {
        std::size_t i = locate(x);
        if (i == slots.size()) return false;
        slots[i].value.reset(); slots[i].deleted = true; --used;
        return true;
    }
};

int main() {
    int q; if (!(std::cin >> q)) return 0;
    OpenAddressSet<long long> set;
    while (q--) {
        char op; long long x; std::cin >> op >> x;
        if (op == '+') set.insert(x);
        else if (op == '-') set.erase(x);
        else if (op == '?') std::cout << (set.contains(x) ? "YES" : "NO") << '\n';
    }
}
