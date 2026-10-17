#include <vector>

int findMinCommon(const std::vector<int>& a, const std::vector<int>& b) {
    int i = static_cast<int>(a.size()) - 1;
    int j = static_cast<int>(b.size()) - 1;

    while (i >= 0 && j >= 0) {
        if (a[i] == b[i]) {
            return a[i];
        }

        if (a[i] < b[j]) {
            --i;
        } else {
            --j;
        }
    }

    return 0;
}