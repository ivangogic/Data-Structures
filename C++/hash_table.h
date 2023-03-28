#ifndef HASH_TABLE_HASH_TABLE_H
#define HASH_TABLE_HASH_TABLE_H

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>

#define fix_index(pos, mod) ({if (pos < 0) pos += mod;})

template<typename K, typename V>
class hash_table {
private:
    template<typename K1, typename V1>
    class Node {
    public:
        std::pair<K1, V1> entry;
        Node<K1, V1>* next;

        explicit Node(std::pair<K1, V1> entry) {
            this->entry = entry;
            this->next = nullptr;
        }

        ~Node() {
            delete this->next;
        }
    };

    int capacity, size;
    std::vector<Node<K, V>*> bucket_array;
    int (*hash_function)(K);

    double get_load_factor() {
        return size * 1.0 / capacity;
    }

    void resize(int new_capacity) {
        int old_capacity = capacity;
        capacity = new_capacity;
        std::vector<Node<K, V>*> new_bucket_array(capacity, nullptr);

        for (int i = 0; i < old_capacity; i++) {
            Node<K, V>* current = bucket_array[i];
            while (current != nullptr) {
                int pos = hash_function(current->entry.first) % capacity;
                fix_index(pos, capacity);
                auto* new_node = new Node<K, V>(current->entry);
                new_node->next = new_bucket_array[pos];
                new_bucket_array[pos] = new_node;
                current = current->next;
            }
        }

        bucket_array = new_bucket_array;
    }

public:
    explicit hash_table(int (*f)(K)) {
        capacity = 16;
        size = 0;
        bucket_array.resize(capacity);
        hash_function = f;
    };

    bool contains(std::pair<K, V> entry) {
        int pos = hash_function(entry.first) % capacity;
        fix_index(pos, capacity);

        bool ind = false;
        Node<K, V>* current = bucket_array[pos];
        while (current != nullptr) {
            if (current->entry.first == entry.first && current->entry.second == entry.second) {
                ind = true;
                break;
            }
            current = current->next;
        }

        return ind;
    }

    void insert(std::pair<K, V> entry) {
        int pos = hash_function(entry.first) % capacity;
        fix_index(pos, capacity);

        bool ind = false;
        Node<K, V>* current = bucket_array[pos];
        while (current != nullptr) {
            if (current->entry.first == entry.first && current->entry.second == entry.second) {
                ind = true;
                break;
            }
            current = current->next;
        }

        if (!ind) {
            size++;
            auto* new_node = new Node<K, V>(entry);
            new_node->next = bucket_array[pos];
            bucket_array[pos] = new_node;
        }

        while (get_load_factor() > 0.75)
            resize(2 * capacity);

    }

    void erase(std::pair<K, V> entry) {
        int pos = hash_function(entry.first) % capacity;
        fix_index(pos, capacity);

        bool ind = false;
        Node<K, V> *current = bucket_array[pos], *previous = nullptr;
        while (current != nullptr) {
            if (current->entry.first == entry.first && current->entry.second == entry.second) {
                ind = true;
                break;
            }
            previous = current;
            current = current->next;
        }

        if (ind) {
            if (previous == nullptr) {
                if (current->next == nullptr)
                    bucket_array[pos] = nullptr;
                else
                    bucket_array[pos] = current->next;
            }
            else {
                if (current->next == nullptr)
                    previous->next = nullptr;
                else
                    previous->next = current->next;
            }
            current->next = nullptr;
            delete(current);
        }

        while (get_load_factor() < 0.2 && capacity > 16)
            resize(capacity / 2);
    }
};


#endif //HASH_TABLE_HASH_TABLE_H
