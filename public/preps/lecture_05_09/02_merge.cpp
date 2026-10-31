#include <iostream>
#include <iterator>
#include <functional>
#include <vector>

void mergeSort(std::vector<int>& a, std::vector<int>& tmp, int l, int r) {
    if (r - l <= 1) return;
    int m = l + (r - l) / 2;
    mergeSort(a, tmp, l, m);
    mergeSort(a, tmp, m, r);
    int i = l, j = m;
    for (int k = l; k < r; ++k) {
        if (j == r || (i < m && a[i] <= a[j])) tmp[k] = a[i++];
        else tmp[k] = a[j++];
    }
    for (int k = l; k < r; ++k) a[k] = tmp[k];
}

// Обобщённый вариант: [first, last), итераторы случайного доступа.
// Тип элемента должен копироваться; comp задаёт порядок.
template <class RandomIt, class Compare = std::less<>>
void mergeSortGeneric(RandomIt first, RandomIt last, Compare comp = {}) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    const auto n = last - first;
    if (n < 2) return;
    std::vector<T> buffer(first, last);
    auto sort = [&](auto&& self, std::ptrdiff_t l, std::ptrdiff_t r) -> void {
        if (r - l < 2) return;
        auto m = l + (r - l) / 2;
        self(self, l, m);
        self(self, m, r);
        auto i = l, j = m;
        for (auto k = l; k < r; ++k) {
            if (j == r || (i < m && !comp(first[j], first[i]))) buffer[k] = first[i++];
            else buffer[k] = first[j++];
        }
        for (auto k = l; k < r; ++k) first[k] = buffer[k];
    };
    sort(sort, 0, n);
}

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n), tmp(n);
    for (int& x : a) std::cin >> x;
    mergeSort(a, tmp, 0, n);
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 == n ? '\n' : ' ');
}
