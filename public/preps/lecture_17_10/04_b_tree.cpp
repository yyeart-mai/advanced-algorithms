#include <iostream>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>

// B-дерево минимальной степени 2: в узле 1..3 ключа (кроме пустого корня).
struct Page {
    std::vector<int> keys;
    std::vector<std::unique_ptr<Page>> children;
    bool leaf() const { return children.empty(); }
};
class BTree {
    std::unique_ptr<Page> root = std::make_unique<Page>();
    static bool contains(const Page* p, int x) {
        std::size_t i = 0;
        while (i < p->keys.size() && p->keys[i] < x) ++i;
        if (i < p->keys.size() && p->keys[i] == x) return true;
        return !p->leaf() && contains(p->children[i].get(), x);
    }
    static void split(Page* p, std::size_t i) {
        auto& child = p->children[i];
        auto sibling = std::make_unique<Page>();
        int middle = child->keys[1];
        sibling->keys.push_back(child->keys[2]);
        child->keys.resize(1);
        if (!child->leaf()) {
            sibling->children.push_back(std::move(child->children[2]));
            sibling->children.push_back(std::move(child->children[3]));
            child->children.resize(2);
        }
        p->keys.insert(p->keys.begin() + i, middle);
        p->children.insert(p->children.begin() + i + 1, std::move(sibling));
    }
    static void insertNonFull(Page* p, int x) {
        std::size_t i = 0;
        while (i < p->keys.size() && p->keys[i] < x) ++i;
        if (p->leaf()) { p->keys.insert(p->keys.begin() + i, x); return; }
        if (p->children[i]->keys.size() == 3) {
            split(p, i);
            if (x > p->keys[i]) ++i;
        }
        insertNonFull(p->children[i].get(), x);
    }
public:
    bool contains(int x) const { return contains(root.get(), x); }
    void insert(int x) {
        if (contains(x)) return;
        if (root->keys.size() == 3) {
            auto newRoot = std::make_unique<Page>();
            newRoot->children.push_back(std::move(root));
            split(newRoot.get(), 0);
            root = std::move(newRoot);
        }
        insertNonFull(root.get(), x);
    }
};

// Обобщённое B-дерево степени Degree >= 2; копируемые ключи и строгий Compare.
template <class Key, std::size_t Degree = 2, class Compare = std::less<Key>>
class BTreeGeneric {
    static_assert(Degree >= 2, "Degree must be at least 2");
    struct PageGeneric {
        std::vector<Key> keys;
        std::vector<std::unique_ptr<PageGeneric>> children;
        bool leaf() const { return children.empty(); }
    };
    std::unique_ptr<PageGeneric> root = std::make_unique<PageGeneric>();
    Compare comp;
    std::size_t position(const PageGeneric* p, const Key& x) const {
        std::size_t i = 0;
        while (i < p->keys.size() && comp(p->keys[i], x)) ++i;
        return i;
    }
    bool equal(const Key& x, const Key& y) const { return !comp(x, y) && !comp(y, x); }
    void split(PageGeneric* p, std::size_t i) {
        auto& child = p->children[i];
        auto sibling = std::make_unique<PageGeneric>();
        Key middle = child->keys[Degree - 1];
        sibling->keys.assign(child->keys.begin() + Degree, child->keys.end());
        child->keys.erase(child->keys.begin() + Degree - 1, child->keys.end());
        if (!child->leaf()) {
            for (std::size_t j = Degree; j < child->children.size(); ++j)
                sibling->children.push_back(std::move(child->children[j]));
            child->children.resize(Degree);
        }
        p->keys.insert(p->keys.begin() + i, middle);
        p->children.insert(p->children.begin() + i + 1, std::move(sibling));
    }
    void add(PageGeneric* p, const Key& x) {
        std::size_t i = position(p, x);
        if (p->leaf()) { p->keys.insert(p->keys.begin() + i, x); return; }
        if (p->children[i]->keys.size() == 2 * Degree - 1) {
            split(p, i);
            if (comp(p->keys[i], x)) ++i;
        }
        add(p->children[i].get(), x);
    }
    void mergeChildren(PageGeneric* p, std::size_t i) {
        auto& left = p->children[i];
        auto& right = p->children[i + 1];
        left->keys.push_back(p->keys[i]);
        left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
        for (auto& child : right->children) left->children.push_back(std::move(child));
        p->keys.erase(p->keys.begin() + i);
        p->children.erase(p->children.begin() + i + 1);
    }
    void borrowLeft(PageGeneric* p, std::size_t i) {
        auto& child = p->children[i];
        auto& sibling = p->children[i - 1];
        child->keys.insert(child->keys.begin(), p->keys[i - 1]);
        p->keys[i - 1] = sibling->keys.back();
        sibling->keys.pop_back();
        if (!sibling->leaf()) {
            child->children.insert(child->children.begin(), std::move(sibling->children.back()));
            sibling->children.pop_back();
        }
    }
    void borrowRight(PageGeneric* p, std::size_t i) {
        auto& child = p->children[i];
        auto& sibling = p->children[i + 1];
        child->keys.push_back(p->keys[i]);
        p->keys[i] = sibling->keys.front();
        sibling->keys.erase(sibling->keys.begin());
        if (!sibling->leaf()) {
            child->children.push_back(std::move(sibling->children.front()));
            sibling->children.erase(sibling->children.begin());
        }
    }
    void ensureChildHasDegreeKeys(PageGeneric* p, std::size_t i) {
        if (i && p->children[i - 1]->keys.size() >= Degree) borrowLeft(p, i);
        else if (i < p->keys.size() && p->children[i + 1]->keys.size() >= Degree) borrowRight(p, i);
        else if (i < p->keys.size()) mergeChildren(p, i);
        else mergeChildren(p, i - 1);
    }
    Key predecessor(PageGeneric* p) const {
        while (!p->leaf()) p = p->children.back().get();
        return p->keys.back();
    }
    Key successor(PageGeneric* p) const {
        while (!p->leaf()) p = p->children.front().get();
        return p->keys.front();
    }
    void remove(PageGeneric* p, const Key& x) {
        std::size_t i = position(p, x);
        if (i < p->keys.size() && equal(p->keys[i], x)) {
            if (p->leaf()) { p->keys.erase(p->keys.begin() + i); return; }
            if (p->children[i]->keys.size() >= Degree) {
                Key previous = predecessor(p->children[i].get());
                p->keys[i] = previous;
                remove(p->children[i].get(), previous);
            } else if (p->children[i + 1]->keys.size() >= Degree) {
                Key next = successor(p->children[i + 1].get());
                p->keys[i] = next;
                remove(p->children[i + 1].get(), next);
            } else { mergeChildren(p, i); remove(p->children[i].get(), x); }
            return;
        }
        if (p->leaf()) return;
        bool wasLast = i == p->keys.size();
        if (p->children[i]->keys.size() == Degree - 1) ensureChildHasDegreeKeys(p, i);
        if (wasLast && i > p->keys.size()) --i;
        remove(p->children[i].get(), x);
    }
    template <class OutputIt> static void emit(const PageGeneric* p, OutputIt& out) {
        for (std::size_t i = 0; i < p->keys.size(); ++i) {
            if (!p->leaf()) emit(p->children[i].get(), out);
            *out++ = p->keys[i];
        }
        if (!p->leaf()) emit(p->children.back().get(), out);
    }
public:
    bool contains(const Key& x) const {
        auto* p = root.get();
        while (p) {
            auto i = position(p, x);
            if (i < p->keys.size() && equal(p->keys[i], x)) return true;
            if (p->leaf()) return false;
            p = p->children[i].get();
        }
        return false;
    }
    void insert(const Key& x) {
        if (contains(x)) return;
        if (root->keys.size() == 2 * Degree - 1) {
            auto newRoot = std::make_unique<PageGeneric>();
            newRoot->children.push_back(std::move(root));
            split(newRoot.get(), 0);
            root = std::move(newRoot);
        }
        add(root.get(), x);
    }
    bool erase(const Key& x) {
        if (!contains(x)) return false;
        remove(root.get(), x);
        if (root->keys.empty() && !root->leaf()) root = std::move(root->children.front());
        return true;
    }
    template <class OutputIt> void copyInorder(OutputIt out) const { emit(root.get(), out); }
};
int main() {
    int q; if (!(std::cin >> q)) return 0; BTreeGeneric<int> t;
    while (q--) { char op; int x; std::cin >> op >> x;
        if (op == '+') t.insert(x);
        else if (op == '-') t.erase(x);
        else if (op == '?') std::cout << (t.contains(x) ? "YES" : "NO") << '\n';
    }
}
