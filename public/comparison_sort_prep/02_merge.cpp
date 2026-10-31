#include <iostream>
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

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n), tmp(n);
    for (int& x : a) std::cin >> x;
    mergeSort(a, tmp, 0, n);
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 == n ? '\n' : ' ');
}
