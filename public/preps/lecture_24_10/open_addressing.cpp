#include <functional>
#include <iostream>
#include <optional>
#include <vector>

// Простая версия для long long. Состояние ячейки: 0 — пустая, 1 — занята,
// 2 — удалена. Удалённую ячейку нельзя считать пустой при поиске.
class SimpleOpenAddressSet {
    std::vector<long long> keys = std::vector<long long>(8);
    std::vector<char> state = std::vector<char>(8, 0);
    int occupied = 0;
    int start(long long x) const { return std::hash<long long>{}(x) % keys.size(); }
    int find(long long x) const {
        int i = start(x);
        while (state[i] != 0) {
            if (state[i] == 1 && keys[i] == x) return i;
            i = (i + 1) % keys.size();
        }
        return -1;
    }
    void grow() {
        auto oldKeys = keys;
        auto oldState = state;
        keys.resize(keys.size() * 2);
        state.assign(keys.size(), 0);
        occupied = 0;
        for (int i = 0; i < static_cast<int>(oldKeys.size()); ++i)
            if (oldState[i] == 1) insert(oldKeys[i]);
    }
public:
    bool contains(long long x) const { return find(x) != -1; }
    void insert(long long x) {
        if (contains(x)) return;
        if ((occupied + 1) * 2 >= static_cast<int>(keys.size())) grow();
        int i = start(x), deleted = -1;
        while (state[i] != 0) {
            if (state[i] == 2 && deleted == -1) deleted = i;
            i = (i + 1) % keys.size();
        }
        if (deleted != -1) i = deleted;
        else ++occupied;
        keys[i] = x;
        state[i] = 1;
    }
    bool erase(long long x) {
        int i = find(x);
        if (i == -1) return false;
        state[i] = 2;
        return true;
    }
};

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
    SimpleOpenAddressSet set;
    while (q--) {
        char op; long long x; std::cin >> op >> x;
        if (op == '+') set.insert(x);
        else if (op == '-') set.erase(x);
        else if (op == '?') std::cout << (set.contains(x) ? "YES" : "NO") << '\n';
    }
}
