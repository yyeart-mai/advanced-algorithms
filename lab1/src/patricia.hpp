#ifndef LAB1_PATRICIA_HPP
#define LAB1_PATRICIA_HPP

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

class Patricia {
public:
    Patricia();
    Patricia(const Patricia&) = delete;
    Patricia& operator=(const Patricia&) = delete;
    ~Patricia();

    std::size_t size() const;
    bool find(std::string key, std::uint64_t& value) const;
    bool insert(std::string key, std::uint64_t value);
    bool erase(std::string key);

    // При успехе возвращают nullptr, при ошибке возвращают сообщение.
    const char* save(const std::string& path) const;
    const char* load(const std::string& path);

#ifdef PATRICIA_CHECK_INVARIANTS
    void assertInvariants() const;
#endif

private:
    struct Node;

    Node* head;
    std::size_t size_ = 0;

    void normalize(std::string& key) const;
    bool getBit(const std::string& key, int index) const;
    int firstDifferentBit(const std::string& a, const std::string& b);
    Node* next(Node* n, const std::string& key) const;
    Node*& edge(Node* n, const std::string& key);
    Node* candidate(const std::string& key) const;
    Node* trueParent(Node* target, const std::string& key) const;
    void clearDescendants(Node* n, int parentBit);
    bool writeRecords(std::FILE* file, const Node* node, int parentBit) const;
    const char* readSnapshot(std::FILE* file);
};

#endif
