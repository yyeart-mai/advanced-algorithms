#include <iostream>
#include <vector>
#include <iomanip>

struct Bucket {
    long double minimum = 0;
    long double maximum = 0;
    bool used = false;
};

int main() {
    int n;
    std::cin >> n;

    if (n < 2) {
        std::cout << 0;
        return 0;
    }

    std::vector<long double> arr(n);
    long double minimum = 0, maximum = 0;

    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
        if (i == 0 || arr[i] < minimum) {
            minimum = arr[i];
        }
        if (i == 0 || arr[i] > maximum) {
            maximum = arr[i];
        }
    }

    if (minimum == maximum) {
        std::cout << 0;
        return 0;
    }

    long double width = (maximum - minimum) / (n - 1);
    std::vector<Bucket> buckets(n);

    for (auto v : arr) {
        int index = (v - minimum) / width;
        if (index >= n) index = n - 1;

        Bucket& b = buckets[index];
        if (!b.used) {
            b.minimum = v;
            b.maximum = v;
            b.used = true;
        } else {
            if (v < b.minimum) b.minimum = v;
            if (v > b.maximum) b.maximum = v;
        }
    }

    long double max_gap = 0, prev_max = 0;
    bool has_prev = false;

    for (auto b : buckets) {
        if (!b.used) continue;

        if (has_prev) {
            long double gap = b.minimum - prev_max;
            if (gap > max_gap) max_gap = gap;
        }

        prev_max = b.maximum;
        has_prev = true;
    }

    std::cout << std::setprecision(15) << max_gap << '\n';
}
