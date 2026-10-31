#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <vector>

// Файлы временных серий хранятся на диске. Число открытых файлов равно числу серий.
// T должен быть тривиально копируемым и поддерживать потоковый ввод/вывод.
template <class T, class Compare = std::less<T>>
void externalSort(std::istream& input, std::ostream& output, std::size_t n,
                  std::size_t chunkSize, Compare comp = {}) {
    static_assert(std::is_trivially_copyable_v<T>, "temporary binary runs require a trivially copyable type");
    if (chunkSize == 0) throw std::invalid_argument("chunkSize must be positive");
    using File = std::unique_ptr<std::FILE, decltype(&std::fclose)>;
    std::vector<File> runs;
    std::vector<T> chunk;
    chunk.reserve(chunkSize);
    for (std::size_t read = 0; read < n;) {
        chunk.clear();
        while (read < n && chunk.size() < chunkSize) {
            T x;
            if (!(input >> x)) throw std::runtime_error("not enough input numbers");
            chunk.push_back(x);
            ++read;
        }
        std::sort(chunk.begin(), chunk.end(), comp);
        File file(std::tmpfile(), &std::fclose);
        if (!file) throw std::runtime_error("cannot create temporary file");
        if (std::fwrite(chunk.data(), sizeof(T), chunk.size(), file.get()) != chunk.size())
            throw std::runtime_error("cannot write temporary file");
        std::rewind(file.get());
        runs.push_back(std::move(file));
    }

    struct Head { T value; std::size_t run; };
    auto greater = [&](const Head& a, const Head& b) { return comp(b.value, a.value); };
    std::priority_queue<Head, std::vector<Head>, decltype(greater)> heap(greater);
    for (std::size_t i = 0; i < runs.size(); ++i) {
        T x;
        if (std::fread(&x, sizeof x, 1, runs[i].get()) == 1) heap.push({x, i});
    }
    while (!heap.empty()) {
        Head head = heap.top(); heap.pop();
        output << head.value << '\n';
        T x;
        if (std::fread(&x, sizeof x, 1, runs[head.run].get()) == 1)
            heap.push({x, head.run});
    }
}

int main() {
    std::size_t n, chunkSize;
    if (!(std::cin >> n >> chunkSize)) return 0;
    externalSort<long long>(std::cin, std::cout, n, chunkSize);
}
