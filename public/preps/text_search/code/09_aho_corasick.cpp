#include <array>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

class AhoCorasick {
    struct Node {
        std::array<int, 256> go;
        int link = 0;
        int exit = -1;
        std::vector<int> ids;
        Node() { go.fill(-1); }
    };
    std::vector<Node> nodes = std::vector<Node>(1);
    std::vector<int> lengths;

    void add(const std::string& pattern, int id) {
        if (pattern.empty()) return;
        int v = 0;
        for (unsigned char c : pattern) {
            if (nodes[v].go[c] == -1) {
                int child = static_cast<int>(nodes.size());
                nodes[v].go[c] = child;
                nodes.emplace_back();
            }
            v = nodes[v].go[c];
        }
        nodes[v].ids.push_back(id);
    }

    void build() {
        std::queue<int> q;
        for (int c = 0; c < 256; ++c) {
            int child = nodes[0].go[c];
            if (child == -1) nodes[0].go[c] = 0;
            else q.push(child);
        }
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            int u = nodes[v].link;
            nodes[v].exit = !nodes[u].ids.empty() ? u : nodes[u].exit;
            for (int c = 0; c < 256; ++c) {
                int child = nodes[v].go[c];
                if (child == -1) nodes[v].go[c] = nodes[u].go[c];
                else {
                    nodes[child].link = nodes[u].go[c];
                    q.push(child);
                }
            }
        }
    }

public:
    explicit AhoCorasick(const std::vector<std::string>& patterns) {
        for (int id = 0; id < static_cast<int>(patterns.size()); ++id) {
            lengths.push_back(static_cast<int>(patterns[id].size()));
            add(patterns[id], id);
        }
        build();
    }

    std::vector<std::pair<int, int>> search(const std::string& text) const {
        std::vector<std::pair<int, int>> answer;
        int v = 0;
        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            v = nodes[v].go[static_cast<unsigned char>(text[i])];
            for (int u = v; u != -1; u = nodes[u].exit)
                for (int id : nodes[u].ids)
                    answer.push_back({i - lengths[id] + 1, id});
        }
        return answer;
    }
};

int main() {
    int k;
    if (!(std::cin >> k)) return 0;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::vector<std::string> patterns(k);
    for (std::string& pattern : patterns) std::getline(std::cin, pattern);
    std::string text;
    std::getline(std::cin, text);
    AhoCorasick matcher(patterns);
    for (auto match : matcher.search(text))
        std::cout << match.first << ' ' << match.second << '\n';
}
