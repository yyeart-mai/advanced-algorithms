#include "patricia.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

#ifdef PATRICIA_CHECK_INVARIANTS
#include <cassert>
#include <unordered_set>
#include <vector>
#endif

namespace {

const char fileSignature[] = "PATRICIA";
const std::size_t signatureSize = sizeof(fileSignature) - 1;
const std::size_t maxKeyLength = 256;

bool writeNumber(std::FILE* file, uint64_t value, std::size_t width) {
    unsigned char bytes[8];
    for (std::size_t i = 0; i < width; ++i) {
        bytes[i] = static_cast<unsigned char>(value);
        value >>= 8;
    }
    return std::fwrite(bytes, 1, width, file) == width;
}

std::size_t readNumber(std::FILE* file, uint64_t& value, std::size_t width) {
    unsigned char bytes[8];
    const std::size_t count = std::fread(bytes, 1, width, file);
    value = 0;
    for (std::size_t i = count; i > 0; --i) {
        value = (value << 8) | bytes[i - 1];
    }
    return count;
}

} // namespace

struct Patricia::Node {
    std::string key;
    uint64_t value;
    int bit;

    Node *left;
    Node *right;

    Node(const std::string& k, uint64_t v, int b)
        : key(k), value(v), bit(b), left(nullptr), right(nullptr) {}
};

void Patricia::normalize(std::string& key) const {
    for (auto& v : key) {
        if (v >= 'A' && v <= 'Z') {
            v = static_cast<char>(v - 'A' + 'a');
        }
    }
}

bool Patricia::getBit(const std::string& key, int index) const {
    if (index <= 0) {
        return false;
    }

    int pos = index - 1;
    std::size_t byteIndex = pos / 8;

    if (byteIndex >= key.size()) {
        return false;
    }

    unsigned char byte = key[byteIndex];
    int shift = 7 - (pos % 8);

    return ((byte >> shift) & 1u) != 0;
}

int Patricia::firstDifferentBit(
    const std::string& a,
    const std::string& b
) {
    std::size_t maxLen = std::max(a.size(), b.size());

    for (std::size_t i = 0; i < maxLen; ++i) {
        unsigned char x = i < a.size() ? a[i] : 0;
        unsigned char y = i < b.size() ? b[i] : 0;

        auto diff = x ^ y;

        if (diff == 0) {
            continue;
        }

        for (int j = 7; j >= 0; --j) {
            if ((diff >> j) & 1u) {
                return i * 8 + 8 - j;
            }
        }
    }

    throw std::logic_error("Keys have identical bit sequences");
}

Patricia::Node* Patricia::next(Node* n, const std::string& key) const {
    return getBit(key, n->bit) ? n->right : n->left;
}

Patricia::Node*& Patricia::edge(Node* n, const std::string& key) {
    return getBit(key, n->bit) ? n->right : n->left;
}

Patricia::Node* Patricia::candidate(const std::string& key) const {
    Node *prev = head;
    Node *curr = head->left;

    while (curr->bit > prev->bit) {
        prev = curr;
        curr = next(curr, key);
    }

    return curr;
}

Patricia::Node* Patricia::trueParent(Node* target, const std::string& key) const {
    Node *parent = head;
    Node *curr = head->left;

    while (curr != target) {
        parent = curr;
        curr = next(curr, key);
    }

    return parent;
}

void Patricia::clearDescendants(Node *n, int parentBit) {
    if (n->bit <= parentBit) {
        return;
    }

    clearDescendants(n->left, n->bit);
    clearDescendants(n->right, n->bit);

    delete n;
}

bool Patricia::writeRecords(std::FILE* file, const Node* node, int parentBit) const {
    if (node->bit <= parentBit) {
        return true;
    }

    const std::size_t length = node->key.size();
    if (!writeNumber(file, length, 2) ||
        std::fwrite(node->key.data(), 1, length, file) != length ||
        !writeNumber(file, node->value, 8)) {
        return false;
    }

    return writeRecords(file, node->left, node->bit) &&
           writeRecords(file, node->right, node->bit);
}

const char* Patricia::readSnapshot(std::FILE* file) {
    char signature[signatureSize];
    const std::size_t count = std::fread(signature, 1, signatureSize, file);
    if (count == 0 && !std::ferror(file)) {
        return nullptr;
    }
    if (count != signatureSize || std::memcmp(signature, fileSignature, signatureSize) != 0) {
        return "Cannot read dictionary signature";
    }

    while (true) {
        uint64_t length;
        const std::size_t lengthBytes = readNumber(file, length, 2);
        if (lengthBytes == 0 && !std::ferror(file)) {
            return nullptr;
        }
        if (lengthBytes != 2 || length == 0 || length > maxKeyLength) {
            return "Invalid dictionary key length";
        }

        std::string key(length, '\0');
        uint64_t value;
        if (std::fread(&key[0], 1, key.size(), file) != key.size() ||
            readNumber(file, value, 8) != 8) {
            return "Cannot read complete dictionary record";
        }
        for (char letter : key) {
            if (!((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))) {
                return "Invalid dictionary key";
            }
        }
        if (!insert(key, value)) {
            return "Duplicate dictionary key";
        }
    }
}

Patricia::Patricia() {
    head = new Node("", 0, -1);
    head->left = head;
    head->right = head;
}

Patricia::~Patricia() {
    if (head->left != head) {
        clearDescendants(head->left, head->bit);
    }

    delete head;
}

std::size_t Patricia::size() const {
    return size_;
}

#ifdef PATRICIA_CHECK_INVARIANTS
void Patricia::assertInvariants() const {
    assert(head && head->bit == -1 && head->right == head && head->left);
    assert((head->left == head) == (size_ == 0));

    std::unordered_set<const Node*> visited;
    std::vector<const Node*> pending;
    if (head->left != head) {
        assert(head->left->bit > head->bit);
        pending.push_back(head->left);
    }

    while (!pending.empty()) {
        const Node* node = pending.back();
        pending.pop_back();
        assert(visited.count(node) == 0);
        visited.insert(node);
        for (const Node* child : {node->left, node->right}) {
            assert(child);
            if (child->bit > node->bit) {
                pending.push_back(child);
            }
        }
    }

    assert(visited.size() == size_);
    for (const Node* node : visited) {
        assert(candidate(node->key) == node);
    }
}
#endif

const char* Patricia::save(const std::string& path) const {
    std::string target = path;
    struct stat info;
    if (::stat(path.c_str(), &info) == 0) {
        if (!S_ISREG(info.st_mode) || ::access(path.c_str(), W_OK) != 0) {
            return "Cannot write dictionary file";
        }

        char resolved[PATH_MAX];
        if (!::realpath(path.c_str(), resolved)) {
            return "Cannot resolve dictionary file path";
        }

        target = resolved;
    } else if (errno != ENOENT) {
        return "Cannot access dictionary file";
    }

    std::string temporaryPath = ".patricia.XXXXXX";
    const std::size_t lastSlash = target.rfind('/');
    if (lastSlash != std::string::npos) {
        temporaryPath.insert(0, target, 0, lastSlash + 1);
    }

    const int descriptor = ::mkstemp(&temporaryPath[0]);
    if (descriptor == -1) {
        return "Cannot create temporary dictionary file";
    }

    std::FILE* file = ::fdopen(descriptor, "wb");
    if (!file) {
        ::close(descriptor);
        std::remove(temporaryPath.c_str());
        return "Cannot open temporary dictionary file";
    }

    const bool written = std::fwrite(fileSignature, 1, signatureSize, file) == signatureSize &&
                         writeRecords(file, head->left, head->bit);
    const bool closed = std::fclose(file) == 0;
    if (!written || !closed) {
        std::remove(temporaryPath.c_str());
        return "Cannot write dictionary file";
    }

    if (std::rename(temporaryPath.c_str(), target.c_str()) != 0) {
        std::remove(temporaryPath.c_str());
        return "Cannot replace dictionary file";
    }

    return nullptr;
}

const char* Patricia::load(const std::string& path) {
    Patricia loaded;
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file && errno != ENOENT) {
        return "Cannot open dictionary file";
    }

    if (file) {
        const char* error;
        try {
            error = loaded.readSnapshot(file);
        } catch (...) {
            std::fclose(file);
            throw;
        }

        const bool closed = std::fclose(file) == 0;
        if (error) {
            return error;
        }
        if (!closed) {
            return "Cannot close dictionary file";
        }
    }

    std::swap(head, loaded.head);
    std::swap(size_, loaded.size_);

    return nullptr;
}

bool Patricia::find(std::string key, uint64_t& value) const {
    normalize(key);

    if (head->left == head) {
        return false;
    }

    Node *x = candidate(key);

    if (x == head || x->key != key) {
        return false;
    }

    value = x->value;
    return true;
}

bool Patricia::insert(std::string key, uint64_t value) {
    normalize(key);

    Node *x = candidate(key);

    if (x != head && x->key == key) {
        return false;
    }

    int b = firstDifferentBit(x->key, key);

    Node *prev = head;
    Node *curr = head->left;

    while (curr->bit > prev->bit && curr->bit < b) {
        prev = curr;
        curr = next(curr, key);
    }

    Node *n = new Node(key, value, b);

    if (getBit(key, b)) {
        n->left = curr;
        n->right = n;
    } else {
        n->left = n;
        n->right = curr;
    }

    edge(prev, key) = n;

    ++size_;
    return true;
}

bool Patricia::erase(std::string key) {
    normalize(key);

    if (head->left == head) {
        return false;
    }

    Node *g = head;
    Node *p = head;
    Node *x = head->left;

    while (x->bit > p->bit) {
        g = p;
        p = x;
        x = next(x, key);
    }

    if (head == x || x->key != key) {
        return false;
    }

    if (size_ == 1) {
        delete x;
        head->left = head;
        size_ = 0;
        return true;
    }

    Node *z = trueParent(x, key);

    if (x == p) {
        Node *c = getBit(key, x->bit) ? x->left : x->right;
        edge(z, key) = c;
    } else {
        Node *c = getBit(key, p->bit) ? p->left : p->right;
        edge(g, key) = c;
        edge(z, key) = p;

        p->left = (x->left == x) ? p : x->left;
        p->right = (x->right == x) ? p : x->right;

        p->bit = x->bit;
    }

    delete x;
    --size_;

    return true;
}
