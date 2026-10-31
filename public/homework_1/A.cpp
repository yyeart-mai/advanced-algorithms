#include <iostream>
#include <vector>
#include <string>

struct Worker {
    std::string name;
    int age;
};

void sortWorkers(std::vector<Worker>& a, int K) {
    int n = a.size();
    std::vector<int> count(K + 1, 0);
    std::vector<Worker> b(n);

    for (auto v : a) {
        ++count[v.age];
    }

    for (int x = 1; x <= K; ++x) {
        count[x] += count[x - 1];
    }

    for (int i = n - 1; i >= 0; --i) {
        int x = a[i].age;
        --count[x];
        b[count[x]] = a[i];
    }

    a.swap(b);
}

int main() {
    int n;
    std::cin >> n;

    std::vector<Worker> arr(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i].name >> arr[i].age;
    }

    const int K = 120;
    sortWorkers(arr, K);

    for (auto v : arr) {
        std::cout << v.name << ' ' << v.age << '\n';
    }

    return 0;
}
