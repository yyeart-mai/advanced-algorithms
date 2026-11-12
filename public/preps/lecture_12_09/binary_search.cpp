#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

// Простая версия: индексы в отсортированном vector<long long>.
int lowerBoundSimple(const std::vector<long long>& a, long long x) {
    int left = 0, right = static_cast<int>(a.size());
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (a[mid] < x) left = mid + 1;
        else right = mid;
    }
    return left;
}

int upperBoundSimple(const std::vector<long long>& a, long long x) {
    int left = 0, right = static_cast<int>(a.size());
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (a[mid] <= x) left = mid + 1;
        else right = mid;
    }
    return left;
}

// [first, last) должен быть отсортирован по comp; нужны итераторы случайного доступа.
template <class RandomIt, class T, class Compare = std::less<>>
RandomIt lowerBound(RandomIt first, RandomIt last, const T& value, Compare comp = {}) {
    while (first < last) {
        RandomIt mid = first + (last - first) / 2;
        if (comp(*mid, value)) first = mid + 1;
        else last = mid;
    }
    return first;
}

template <class RandomIt, class T, class Compare = std::less<>>
RandomIt upperBound(RandomIt first, RandomIt last, const T& value, Compare comp = {}) {
    while (first < last) {
        RandomIt mid = first + (last - first) / 2;
        if (!comp(value, *mid)) first = mid + 1;
        else last = mid;
    }
    return first;
}

int main() {
    int n, q;
    if (!(std::cin >> n >> q)) return 0;
    std::vector<long long> a(n);
    for (auto& x : a) std::cin >> x;
    while (q--) {
        long long x; std::cin >> x;
        int lo = lowerBoundSimple(a, x);
        int hi = upperBoundSimple(a, x);
        std::cout << (lo == n ? -1 : lo) << ' '
                  << (hi == n ? -1 : hi) << ' '
                  << hi - lo << '\n';
    }
}
