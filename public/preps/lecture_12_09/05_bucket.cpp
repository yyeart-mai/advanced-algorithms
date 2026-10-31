// Ввод: n, затем n чисел double из [0, 1). n <= 1000000.
#include <iomanip>
#include <iostream>
#include <iterator>
#include <functional>
#include <utility>
#include <vector>
using namespace std;

void insertionSort(vector<double>& a) {
    for (int i = 1; i < int(a.size()); ++i) {
        double x = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > x) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = x;
    }
}

void bucketSort(vector<double>& a) {
    int n = int(a.size());
    if (n == 0) return;
    vector<vector<double>> buckets(n);
    for (int i = 0; i < n; ++i) {
        int index = int(a[i] * n);
        if (index >= n) index = n - 1; // Защита от округления double.
        buckets[index].push_back(a[i]);
    }
    int pos = 0;
    for (int index = 0; index < n; ++index) {
        insertionSort(buckets[index]);
        for (int j = 0; j < int(buckets[index].size()); ++j) {
            a[pos] = buckets[index][j];
            ++pos;
        }
    }
}

// [first, last): копируемые элементы, key(item) в [0, 1), comp сравнивает элементы.
template <class RandomIt, class KeyFn, class Compare = std::less<>>
void bucketSortGeneric(RandomIt first, RandomIt last, KeyFn key, Compare comp = {}) {
    using T = typename iterator_traits<RandomIt>::value_type;
    const size_t n = static_cast<size_t>(last - first);
    if (!n) return;
    vector<vector<T>> buckets(n);
    for (auto it = first; it != last; ++it) {
        size_t index = static_cast<size_t>(key(*it) * n);
        if (index >= n) index = n - 1;
        buckets[index].push_back(*it);
    }
    auto out = first;
    for (auto& bucket : buckets) {
        for (size_t i = 1; i < bucket.size(); ++i) {
            T x = std::move(bucket[i]);
            size_t j = i;
            while (j && comp(x, bucket[j - 1])) {
                bucket[j] = std::move(bucket[j - 1]);
                --j;
            }
            bucket[j] = std::move(x);
        }
        for (auto& x : bucket) *out++ = std::move(x);
    }
}

int main() {
    int n;
    if (!(cin >> n) || n < 0 || n > 1000000) return 1;
    vector<double> a(n);
    for (int i = 0; i < n; ++i) {
        if (!(cin >> a[i]) || !(a[i] >= 0 && a[i] < 1)) return 1;
    }
    bucketSort(a);
    cout << setprecision(17);
    for (int i = 0; i < n; ++i) cout << a[i] << ' ';
    cout << '\n';
}
