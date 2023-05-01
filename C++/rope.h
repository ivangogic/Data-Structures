#ifndef DATA_STRUCTURES_ROPE_H
#define DATA_STRUCTURES_ROPE_H

#include <iostream>
#include <algorithm>
#include <exception>
#include <string>
#include <vector>
#include <stack>


class Rope {
private:
    class Node {
    public:
        Node *left, *right;
        std::string *str;
        int size, size_left;

        Node() = default;

        explicit Node(std::string *s) {
            this->left = this->right = nullptr;
            this->str = s;
            this->size = s->size();
            this->size_left = 0;
        }

        void destroy_node() {
            this->left = this->right = nullptr;
            this->str = nullptr;
            this->size = this->size_left = 0;
        }

        static Node* concat(Node *node1, Node *node2) {
            Node *new_node = new Node();
            new_node->left = node1;
            new_node->right = node2;
            new_node->size = (node1 ? node1->size : 0) + (node2 ? node2->size : 0);
            new_node->size_left = (node1 ? node1->size : 0);

            return new_node;
        }

        std::pair<Node *, Node *> split(int i) {
            if (this->str) {
                if (i >= this->str->size())
                    throw std::out_of_range("split: index out of range");
                Node *left_node = new Node(new std::string(this->str->substr(0, i)));
                Node *right_node = new Node(new std::string(this->str->substr(i)));
                this->destroy_node();

                return {left_node, right_node};
            }
            if (i < this->size_left) {
                std::pair<Node *, Node *> left_split = this->left->split(i);
                delete(this->left);

                Node *left_node = left_split.first;
                Node *right_node = concat(left_split.second, this->right);
                this->destroy_node();

                return {left_node, right_node};
            }
            else if (i > this->size_left) {
                std::pair<Node *, Node *> right_split = this->right->split(i - this->size_left);
                delete(this->right);

                Node *left_node = concat(this->left, right_split.first);
                Node *right_node = right_split.second;
                this->destroy_node();

                return {left_node, right_node};
            }
            else {
                Node *left_node = this->left;
                Node *right_node = this->right;
                this->destroy_node();

                return {left_node, right_node};
            }
        }
    };

    Node *root;

    explicit Rope(Node *root) {
        this->root = root;
    }

    std::vector<Node *> collect_leaves() {
        std::vector<Node *> leaves;
        std::stack<Node *> stack;
        stack.push(this->root);

        while (!stack.empty()) {
            Node *node = stack.top();
            stack.pop();
            if (node->str) {
                leaves.push_back(new Node(new std::string(*node->str)));
            }
            else {
                if (node->right)
                    stack.push(node->right);
                if (node->left)
                    stack.push(node->left);
            }
        }

        return leaves;
    }

    Node* node_from_list(std::vector<Node *>& leaves, int l, int r) {
        if (l == r)
            return leaves[l];
        if (r == l + 1) {
            Node *curr = new Node();
            curr->left = leaves[l];
            curr->right = leaves[r];
            curr->size = leaves[l]->size + leaves[r]->size;
            curr->size_left = leaves[l]->size;

            return curr;
        }

        int s = l + (r - l) / 2;

        Node *left_node = node_from_list(leaves, l, s);
        Node *right_node = node_from_list(leaves, s + 1, r);

        Node *curr = new Node();
        curr->left = left_node;
        curr->right = right_node;
        curr->size = left_node->size + right_node->size;
        curr->size_left = left_node->size;

        return curr;
    }

    void prepend(std::string *s) {
        if (this->root->size == 0) {
            this->root->str = s;
            this->root->size = s->size();
            return;
        }
        Node *curr = root;
        while (!curr->str) {
            curr->size += s->size();
            curr->size_left += s->size();
            curr = curr->left;
        }
        Node *left_node = new Node(s);
        Node *right_node = new Node(curr->str);
        curr->str = nullptr;
        curr->left = left_node;
        curr->right = right_node;
        curr->size = left_node->size + right_node->size;
        curr->size_left = left_node->size;
    }

    void append(std::string *s) {
        Node *curr = root;
        while (!curr->str) {
            curr->size += s->size();
            curr->size_left += s->size();
            curr = curr->right;
        }
        Node *left_node = new Node(curr->str);
        Node *right_node = new Node(s);
        curr->str = nullptr;
        curr->left = left_node;
        curr->right = right_node;
        curr->size = left_node->size + right_node->size;
        curr->size_left = left_node->size;
    }

public:

    Rope() {
        this->root = new Node();
    }

    explicit Rope(std::string* s) {
        this->root = new Node(s);
    }

    char atIndex(int i) {
        Node *curr = this->root;
        while (true) {
            if (curr->str) {
                if (curr->size_left <= i) {
                    throw std::out_of_range("atIndex: index out of range");
                }
                return curr->str->at(i);
            }
            if (curr->size_left >= i + 1) {
                curr = curr->left;
            }
            else {
                i -= curr->size_left;
                curr = curr->right;
            }
        }
    }

    void insert(std::string *s, int i) {
        if (i > this->root->size || i < 0)
            throw std::out_of_range("insert: index out of range");
        if (i == 0) {
            prepend(s);
        }
        else if (i == this->root->size) {
            append(s);
        }
        else {
            std::pair<Node *, Node *> init_split = this->root->split(i);
            Node *to_insert = new Node(s);
            root = Node::concat(Node::concat(init_split.first, to_insert), init_split.second);
        }

    }

    void erase(int i, int j) {
        if (i + j >= this->root->size)
            throw std::out_of_range("erase: out of range");
        std::pair<Node *, Node *> init_split1 = this->root->split(i);
        this->root->destroy_node();
        std::pair<Node *, Node *> init_split2 = init_split1.second->split(j);
        init_split1.second->destroy_node();
        delete(init_split1.second);
        this->root = Node::concat(init_split1.first, init_split2.second);
    }

    std::string report(int i, int j) {
        std::stack<std::pair<Node *, std::pair<int, int>>> stack;
        stack.push({this->root, {i, j}});
        std::string rep;
        rep.reserve(j);

        while (!stack.empty()) {
            std::pair<Node *, std::pair<int, int>> temp = stack.top();
            stack.pop();
            Node *curr = temp.first;
            i = temp.second.first;
            j = temp.second.second;
            if (i + j - 1 >= curr->size)
                throw std::out_of_range("report: index out of range");
            if (curr->str) {
                if (i + j <= curr->size)
                    rep += curr->str->substr(i, j);
                else
                    throw std::out_of_range("report: index out of range?");
            }
            else {
                if (curr->size_left >= i + j) {
                    stack.push({curr->left, {i, j}});
                }
                else {
                    if (i >= curr->size_left) {
                        stack.push({curr->right, {i - curr->size_left, j}});
                    }
                    else {
                        stack.push({curr->right, {0, i + j - curr->size_left}});
                        stack.push({curr->left, {i, curr->size_left - i}});
                    }
                }
            }
        }

        return rep;
    }

    int get_size() const {
        return this->root->size;
    }

    void rebalance() {
        std::vector<Node *> leaves = collect_leaves();
        delete(this->root);

        root = node_from_list(leaves, 0, leaves.size() - 1);
    }
};


#endif //DATA_STRUCTURES_ROPE_H
