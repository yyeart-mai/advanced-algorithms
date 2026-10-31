#include <iostream>
#include <functional>
#include <iterator>
#include <vector>

class SegmentTree {
    int n;
    std::vector<long long> tree;
public:
    explicit SegmentTree(const std::vector<long long>& a) : n(static_cast<int>(a.size())), tree(2 * n) {
        for (int i = 0; i < n; ++i) tree[n + i] = a[i];
        for (int i = n - 1; i > 0; --i) tree[i] = tree[2 * i] + tree[2 * i + 1];
    }
    void set(int p, long long value) {
        for (tree[p += n] = value; p > 1; p >>= 1) tree[p >> 1] = tree[p] + tree[p ^ 1];
    }
    long long sum(int l, int r) const { // [l, r)
        long long result = 0;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) result += tree[l++];
            if (r & 1) result += tree[--r];
        }
        return result;
    }
};

// Тип T и операция combine образуют моноид с нейтральным элементом identity.
template <class T, class Combine = std::plus<T>>
class SegmentTreeGeneric {
    std::vector<T> tree;
    std::size_t n;
    T identity;
    Combine combine;
public:
    template <class ForwardIt>
    SegmentTreeGeneric(ForwardIt first, ForwardIt last, T neutral, Combine operation = {})
        : n(static_cast<std::size_t>(std::distance(first, last))), identity(neutral), combine(operation) {
        tree.assign(2 * n, identity);
        std::size_t i = 0;
        for (auto it = first; it != last; ++it) tree[n + i++] = *it;
        for (i = n; i > 1; --i) tree[i - 1] = combine(tree[2 * (i - 1)], tree[2 * (i - 1) + 1]);
    }
    void set(std::size_t i, const T& x) {
        for (tree[i += n] = x; i > 1; i /= 2)
            tree[i / 2] = combine(tree[2 * (i / 2)], tree[2 * (i / 2) + 1]);
    }
    T query(std::size_t l, std::size_t r) const {
        T left = identity, right = identity;
        for (l += n, r += n; l < r; l /= 2, r /= 2) {
            if (l & 1) left = combine(left, tree[l++]);
            if (r & 1) right = combine(tree[--r], right);
        }
        return combine(left, right);
    }
};

int main() {
    int n, q;
    if (!(std::cin >> n >> q)) return 0;
    std::vector<long long> a(n);
    for (auto& x : a) std::cin >> x;
    SegmentTree st(a);
    while (q--) {
        char op; int x; long long y;
        std::cin >> op >> x >> y;
        if (op == 's') st.set(x, y);
        else if (op == 'q') std::cout << st.sum(x, static_cast<int>(y)) << '\n';
    }
}
