#include <iostream>
#include <string>

struct Node {
    int val;
    int size = 1;
    int height = 1;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(int val) : val(val) {}
};

int size(Node* n) {
    return n == nullptr ? 0 : n->size;
}

int height(Node *n) {
    return n == nullptr ? 0 : n->height;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void update(Node* n) {
    n->size = 1 + size(n->left) + size(n->right);
    n->height = 1 + max(height(n->left), height(n->right));
}

Node* rightRotate(Node* n) {
    Node* root = n->left;
    n->left = root->right;
    root->right = n;

    update(n);
    update(root);
    return root;
}

Node* leftRotate(Node *n) {
    Node* root = n->right;
    n->right = root->left;
    root->left = n;

    update(n);
    update(root);
    return root;
}

Node* balance(Node* n) {
    update(n);

    if (height(n->right) - height(n->left) == 2) {
        if (height(n->right->left) > height(n->right->right)) {
            n->right = rightRotate(n->right);
        }
        return leftRotate(n);
    }

    if (height(n->left) - height(n->right) == 2) {
        if (height(n->left->right) > height(n->left->left)) {
            n->left = leftRotate(n->left);
        }
        return rightRotate(n);
    }

    return n;
}

Node* insert(Node* n, int val) {
    if (n == nullptr) return new Node(val);

    if (val < n->val) {
        n->left = insert(n->left, val);
    } else if (val > n->val) {
        n->right = insert(n->right, val);
    } else {
        return n;
    }

    return balance(n);
}

Node* findMin(Node* n) {
    while (n->left != nullptr) {
        n = n->left;
    }

    return n;
}

Node* removeMin(Node* n) {
    if (n->left == nullptr) return n->right;

    n->left = removeMin(n->left);
    return balance(n);
}

Node* erase(Node* n, int val) {
    if (val < n->val) {
        n->left = erase(n->left, val);
    } else if (val > n->val) {
        n->right = erase(n->right, val);
    } else {
        Node* left = n->left;
        Node* right = n->right;
        delete n;

        if (right == nullptr) return left;

        Node* minimum = findMin(right);
        minimum->right = removeMin(right);
        minimum->left = left;
        return balance(minimum);
    }

    return balance(n);
}

int kth(Node* n, int k) {
    int left_size = size(n->left);

    if (k <= left_size) return kth(n->left, k);
    if (k == left_size + 1) return n->val;
    return kth(n->right, k - left_size - 1);
}

int less(Node* n, int x) {
    int result = 0;

    while (n != nullptr) {
        if (x <= n->val) {
            n = n->left;
        } else {
            result += size(n->left) + 1;
            n = n->right;
        }
    }

    return result;
}

int main() {
    int m;
    std::cin >> m;

    Node* root = nullptr;

    for (int i = 0; i < m; ++i) {
        std::string operation;
        int val;

        std::cin >> operation >> val;

        if (operation == "insert") {
            root = insert(root, val);
        }
        else if (operation == "delete") {
            root = erase(root, val);
        }
        else if (operation == "kth") {
            std::cout << kth(root, val) << '\n';
        }
        else if (operation == "less") {
            std::cout << less(root, val) << '\n';
        }
    }

    return 0;
}
