#include <iostream>
#include <string>

typedef unsigned long long ull;

bool powNotGreater(ull base, int k, ull limit) {
    if (base == 0) return true;

    ull result = 1;

    for (int i = 0; i < k; ++i) {
        if (result > limit / base) return false;
        result *= base;
    }

    return true;
}

int main() {
    ull a;
    int k;

    std::cin >> a >> k;

    ull left = 0, right = 1 << 64;

    while (right - left > 1) {
        ull mid = left + (right - left) / 2;
        if (powNotGreater(mid, k, a)) {
            left = mid;
        } else {
            right = mid;
        }
    }

    std::cout << left << '\n';
}
