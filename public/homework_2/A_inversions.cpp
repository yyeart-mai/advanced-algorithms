#include <iostream>
#include <vector>

typedef long long ll;

void mergeCount(std::vector<int>& a, std::vector<int>& tmp, int l, int r, ll& val) {
    if (r - l <= 1) return;
    int m = l + (r - l) / 2;
    mergeCount(a, tmp, l, m, val);
    mergeCount(a, tmp, m, r, val);
    int i = l, j = m;
    for (int k = l; k < r; ++k) {
        if (j == r || (i < m && a[i] <= a[j])) {
            tmp[k] = a[i++];
        } else {
            tmp[k] = a[j++];
            val += (m - i);
        }
    }
    for (int k = l; k < r; ++k) {
        a[k] = tmp[k];
    }
}

ll countInversions(std::vector<int>& a) {
    std::vector<int> tmp(a.size());
    ll counter = 0;
    mergeCount(a, tmp, 0, a.size(), counter);

    return counter;
}

int main() {
    int n;
    std::cin >> n;

    std::vector<int> a(n);
    for (auto& v : a) {
        std::cin >> v;
    }

    std::cout << countInversions(a);
    return 0;
}
