#include <iostream>
#include <vector>
#include <utility>

class Heap {
    std::vector<int> a;
    bool is_max;

    bool higher(int x, int y) {
        return is_max ? x > y : x < y;
    }

    void shift_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (!higher(a[i], a[parent])) break;
            std::swap(a[i], a[parent]);
            i = parent;
        }
    }

    void shift_down(int i) {
        int n = a.size();

        while (true) {
            int best = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n && higher(a[left], a[best])) {
                best = left;
            }
            if (right < n && higher(a[right], a[best])) {
                best = right;
            }
            if (best == i) break;

            std::swap(a[i], a[best]);
            i = best;
        }
    }

public:
    Heap(bool is_max) : is_max(is_max) {}

    bool empty() {
        return a.empty();
    }

    int size() {
        return a.size();
    }

    int top() {
        return a[0];
    }

    void push(int x) {
        a.push_back(x);
        shift_up(a.size() - 1);
    }

    int pop() {
        int result = a[0];
        a[0] = a.back();
        a.pop_back();

        if (!a.empty()) {
            shift_down(0);
        }

        return result;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    Heap left(true);
    Heap right(false);

    for (int i = 0; i < n; ++i) {
        int x;
        std::cin >> x;

        if (left.empty() || x <= left.top()) {
            left.push(x);
        } else {
            right.push(x);
        }

        if (left.size() > right.size() + 1) {
            right.push(left.pop());
        }
        if (right.size() > left.size()) {
            left.push(right.pop());
        }

        if (i > 0) std::cout << ' ';
        std::cout << left.top();
    }

    return 0;
}
