// Ввод: n, затем n пар: key value. value — слово без пробелов.
// key из [-1000000000, 1000000000], n <= 1000000.
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

struct Record {
    int key;
    string value;
};

// Прибавляем одну константу ко всем ключам: порядок не меняется.
long long sortKey(const Record& item) {
    return item.key + 1000000000LL;
}

void radixSort(vector<Record>& a) {
    int n = int(a.size());
    long long maximum = 0;
    for (int i = 0; i < n; ++i) {
        if (sortKey(a[i]) > maximum) maximum = sortKey(a[i]);
    }
    vector<Record> b(n);
    for (long long exp = 1; exp <= maximum; exp *= 10) {
        vector<int> count(10, 0);
        for (int i = 0; i < n; ++i) {
            int digit = int((sortKey(a[i]) / exp) % 10);
            ++count[digit];
        }
        for (int digit = 1; digit < 10; ++digit) count[digit] += count[digit - 1];
        for (int i = n - 1; i >= 0; --i) {
            int digit = int((sortKey(a[i]) / exp) % 10);
            --count[digit];
            // Ключ и значение перемещаются вместе. digit вычислен до move.
            b[count[digit]] = std::move(a[i]);
        }
        a.swap(b);
    }
}

// Тот же LSD для произвольных копируемых записей и целочисленного поля.
template <class RandomIt, class KeyFn>
void radixRecordsGeneric(RandomIt first, RandomIt last, KeyFn key) {
    using T = typename iterator_traits<RandomIt>::value_type;
    using Key = std::decay_t<decltype(key(*first))>;
    static_assert(std::is_integral_v<Key> && !std::is_same_v<Key, bool>, "integer key required");
    using U = std::make_unsigned_t<Key>;
    const size_t n = static_cast<size_t>(last - first);
    vector<T> out(first, last);
    constexpr int bits = std::numeric_limits<U>::digits;
    const U sign = std::is_signed_v<Key> ? U(1) << (bits - 1) : U(0);
    for (int shift = 0; shift < bits; shift += 8) {
        size_t count[256] = {};
        auto digit = [&](const T& x) { return static_cast<unsigned>(((static_cast<U>(key(x)) ^ sign) >> shift) & U(255)); };
        for (auto it = first; it != last; ++it) ++count[digit(*it)];
        for (size_t d = 1; d < 256; ++d) count[d] += count[d - 1];
        for (size_t i = n; i > 0; --i) out[--count[digit(first[i - 1])]] = first[i - 1];
        for (size_t i = 0; i < n; ++i) first[i] = out[i];
    }
}

int main() {
    int n;
    if (!(cin >> n) || n < 0 || n > 1000000) return 1;
    vector<Record> a(n);
    for (int i = 0; i < n; ++i) {
        if (!(cin >> a[i].key >> a[i].value) || a[i].key < -1000000000 || a[i].key > 1000000000) return 1;
    }
    radixSort(a);
    for (int i = 0; i < n; ++i) cout << a[i].key << ' ' << a[i].value << '\n';
}
