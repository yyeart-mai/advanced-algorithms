#include <algorithm>
#include <functional>
#include <iterator>
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

// Обобщённый вариант: [first, last), итераторы случайного доступа.
template <class RandomIt, class Compare = std::less<>>
void quickSortGeneric(RandomIt first, RandomIt last, Compare comp = {}) {
    while (last - first > 1) {
        using T = typename std::iterator_traits<RandomIt>::value_type;
        T pivot = first[(last - first) / 2]; // Требуется копируемый тип.
        RandomIt lt = first, i = first, gt = last;
        while (i < gt) {
            if (comp(*i, pivot)) std::iter_swap(i++, lt++);
            else if (comp(pivot, *i)) std::iter_swap(i, --gt);
            else ++i;
        }
        if (lt - first < last - gt) {
            quickSortGeneric(first, lt, comp);
            first = gt;
        } else {
            quickSortGeneric(gt, last, comp);
            last = lt;
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
