#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

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
        auto lo = lowerBound(a.begin(), a.end(), x);
        auto hi = upperBound(a.begin(), a.end(), x);
        std::cout << (lo == a.end() ? -1 : int(lo - a.begin())) << ' '
                  << (hi == a.end() ? -1 : int(hi - a.begin())) << ' '
                  << hi - lo << '\n';
    }
}
