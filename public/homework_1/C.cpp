#include <iostream>
#include <vector>

typedef long long ll;

int lower_bound(const std::vector<ll>& a, ll x) {
    int left = 0, right = a.size();

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (a[mid] < x) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

int upper_bound(const std::vector<ll>& a, ll x) {
    int left = 0, right = a.size();

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (a[mid] <= x) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;

    std::vector<ll> arr(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    for (int i = 0; i < q; ++i) {
        ll x;
        std::cin >> x;

        int lowerBound = lower_bound(arr, x);
        int upperBound = upper_bound(arr, x);
        int count = upperBound - lowerBound;

        if (lowerBound == n) {
            lowerBound = -1;
        }
        if (upperBound == n) {
            upperBound = -1;
        }

        std::cout << lowerBound << ' ' << upperBound << ' ' << count << '\n';
    }

    return 0;
}
