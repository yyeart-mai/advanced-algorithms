#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

template <class T, class Compare = std::less<T>>
class BinaryHeap {
    std::vector<T> a;
    Compare comp; // comp(parent, child) означает, что child должен быть выше.
    void up(std::size_t i) {
        while (i) {
            std::size_t p = (i - 1) / 2;
            if (!comp(a[p], a[i])) break;
            std::swap(a[p], a[i]); i = p;
        }
    }
    void down(std::size_t i) {
        for (;;) {
            std::size_t best = i, l = 2 * i + 1, r = l + 1;
            if (l < a.size() && comp(a[best], a[l])) best = l;
            if (r < a.size() && comp(a[best], a[r])) best = r;
            if (best == i) break;
            std::swap(a[i], a[best]); i = best;
        }
    }
public:
    template <class InputIt>
    BinaryHeap(InputIt first, InputIt last, Compare compare = {}) : a(first, last), comp(compare) {
        for (std::size_t i = a.size() / 2; i > 0; --i) down(i - 1);
    }
    BinaryHeap() = default;
    bool empty() const { return a.empty(); }
    const T& top() const { return a.front(); }
    void push(const T& x) { a.push_back(x); up(a.size() - 1); }
    T pop() { T result = std::move(a.front()); a.front() = std::move(a.back()); a.pop_back(); if (!a.empty()) down(0); return result; }
};

// In-place heap sort для итераторов случайного доступа. Итог — по comp.
template <class RandomIt, class Compare = std::less<>>
void heapSort(RandomIt first, RandomIt last, Compare comp = {}) {
    auto n = last - first;
    auto down = [&](auto start, auto length) {
        auto i = start;
        for (;;) {
            auto best = i, l = 2 * i + 1, r = l + 1;
            if (l < length && comp(first[best], first[l])) best = l;
            if (r < length && comp(first[best], first[r])) best = r;
            if (best == i) break;
            std::iter_swap(first + i, first + best); i = best;
        }
    };
    for (auto i = n / 2; i > 0; --i) down(i - 1, n);
    for (auto size = n; size > 1; --size) { std::iter_swap(first, first + size - 1); down(0, size - 1); }
}

int main() {
    int n; if (!(std::cin >> n)) return 0;
    std::vector<int> a(n);
    for (int& x : a) std::cin >> x;
    heapSort(a.begin(), a.end());
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 == n ? '\n' : ' ');
}
