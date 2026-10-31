#include <iostream>
#include <iterator>
#include <vector>

class Fenwick {
    std::vector<long long> bit;
public:
    explicit Fenwick(int n) : bit(n + 1) {}
    void add(int i, long long delta) {
        for (++i; i < static_cast<int>(bit.size()); i += i & -i) bit[i] += delta;
    }
    long long prefix(int r) const { // сумма [0, r)
        long long s = 0;
        for (; r > 0; r -= r & -r) s += bit[r];
        return s;
    }
    long long sum(int l, int r) const { return prefix(r) - prefix(l); }
};

// T поддерживает 0, += и вычитание. Конструктор принимает диапазон итераторов.
template <class T>
class FenwickGeneric {
    std::vector<T> bit;
public:
    template <class ForwardIt>
    FenwickGeneric(ForwardIt first, ForwardIt last)
        : bit(static_cast<std::size_t>(std::distance(first, last)) + 1, T{}) {
        std::size_t i = 0;
        for (auto it = first; it != last; ++it) add(i++, *it);
    }
    void add(std::size_t i, const T& delta) {
        for (++i; i < bit.size(); i += i & -i) bit[i] += delta;
    }
    T prefix(std::size_t r) const {
        T result{};
        for (; r > 0; r -= r & -r) result += bit[r];
        return result;
    }
    T sum(std::size_t l, std::size_t r) const { return prefix(r) - prefix(l); }
};

int main() {
    int n, q;
    if (!(std::cin >> n >> q)) return 0;
    Fenwick fw(n);
    for (int i = 0; i < n; ++i) { long long x; std::cin >> x; fw.add(i, x); }
    while (q--) {
        char op; int x; long long y;
        std::cin >> op >> x >> y;
        if (op == 'a') fw.add(x, y);
        else if (op == 'q') std::cout << fw.sum(x, static_cast<int>(y)) << '\n';
    }
}
