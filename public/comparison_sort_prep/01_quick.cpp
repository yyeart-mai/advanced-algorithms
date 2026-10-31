#include <algorithm>
#include <iostream>
#include <vector>

// Трёхчастное разбиение: [l, lt) < pivot, [lt, gt] == pivot, (gt, r] > pivot.
void quickSort(std::vector<int>& a, int l, int r) {
    while (l < r) {
        int pivot = a[l + (r - l) / 2];
        int lt = l, i = l, gt = r;
        while (i <= gt) {
            if (a[i] < pivot) std::swap(a[i++], a[lt++]);
            else if (a[i] > pivot) std::swap(a[i], a[gt--]);
            else ++i;
        }
        // Сначала рекурсия для меньшей части: глубина стека O(log n).
        if (lt - l < r - gt) {
            quickSort(a, l, lt - 1);
            l = gt + 1;
        } else {
            quickSort(a, gt + 1, r);
            r = lt - 1;
        }
    }
}

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n);
    for (int& x : a) std::cin >> x;
    if (n) quickSort(a, 0, n - 1);
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 == n ? '\n' : ' ');
}
