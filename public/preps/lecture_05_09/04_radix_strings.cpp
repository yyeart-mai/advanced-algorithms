// Ввод: n, затем n слов из строчных латинских букв, длины могут различаться.
// n и длина каждого слова <= 1000000. Пустая строка через cin >> не читается.
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
using namespace std;

int digitAt(const string& s, int pos) {
    if (pos >= int(s.size())) return 0; // Конец строки меньше любой буквы.
    return s[pos] - 'a' + 1; // a -> 1, ..., z -> 26 (ASCII).
}

void radixSort(vector<string>& a) {
    int n = int(a.size());
    int length = 0;
    for (int i = 0; i < n; ++i) {
        if (int(a[i].size()) > length) length = int(a[i].size());
    }
    vector<string> b(n);
    // Индекс позиции от начала строки; проходы — справа налево.
    for (int pos = length - 1; pos >= 0; --pos) {
        vector<int> count(27, 0);
        for (int i = 0; i < n; ++i) ++count[digitAt(a[i], pos)];
        for (int digit = 1; digit < 27; ++digit) count[digit] += count[digit - 1];
        for (int i = n - 1; i >= 0; --i) {
            int digit = digitAt(a[i], pos);
            --count[digit];
            b[count[digit]] = std::move(a[i]);
        }
        a.swap(b);
    }
}

// Для произвольных копируемых записей со строковым ключом.
// length(item) задаёт длину; symbol(item, pos) возвращает 1..alphabet.
// Отсутствующий символ получает ключ 0 и сортируется раньше остальных.
template <class RandomIt, class LengthFn, class SymbolFn>
void radixStringsGeneric(RandomIt first, RandomIt last, size_t alphabet,
                         LengthFn length, SymbolFn symbol) {
    using T = typename iterator_traits<RandomIt>::value_type;
    const size_t n = static_cast<size_t>(last - first);
    size_t maxLength = 0;
    for (auto it = first; it != last; ++it) maxLength = max(maxLength, static_cast<size_t>(length(*it)));
    vector<T> out(first, last);
    for (size_t pos = maxLength; pos > 0; --pos) {
        auto digit = [&](const T& x) { return pos - 1 < length(x) ? static_cast<size_t>(symbol(x, pos - 1)) : size_t(0); };
        vector<size_t> count(alphabet + 1);
        for (auto it = first; it != last; ++it) ++count[digit(*it)];
        for (size_t d = 1; d < count.size(); ++d) count[d] += count[d - 1];
        for (size_t i = n; i > 0; --i) out[--count[digit(first[i - 1])]] = first[i - 1];
        for (size_t i = 0; i < n; ++i) first[i] = out[i];
    }
}

int main() {
    int n;
    if (!(cin >> n) || n < 0 || n > 1000000) return 1;
    vector<string> a(n);
    for (int i = 0; i < n; ++i) {
        if (!(cin >> a[i]) || a[i].size() > 1000000) return 1;
        for (int j = 0; j < int(a[i].size()); ++j) {
            if (a[i][j] < 'a' || a[i][j] > 'z') return 1;
        }
    }
    radixSort(a);
    for (int i = 0; i < n; ++i) cout << a[i] << '\n';
}
