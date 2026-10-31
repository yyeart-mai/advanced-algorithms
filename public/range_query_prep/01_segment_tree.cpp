#include <iostream>
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
