#include <iostream>
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
