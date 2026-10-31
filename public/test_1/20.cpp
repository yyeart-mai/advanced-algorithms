#include <vector>

int findMinCommon(const std::vector<int>& a, const std::vector<int>& b) {
    // Предусловие: оба массива отсортированы по невозрастанию.
    // Идём от меньших значений к большим, чтобы первое совпадение было минимумом.
    std::size_t i = a.size();
    std::size_t j = b.size();

    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            return a[i - 1];
        }

        if (a[i - 1] < b[j - 1]) {
            --i;
        } else {
            --j;
        }
    }

    return 0;
}
