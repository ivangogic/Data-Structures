#ifndef DATA_STRUCTURES_PQ_H
#define DATA_STRUCTURES_PQ_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>

template<typename K>
class priority_queue {
private:
    std::vector<K> data;
    int first_free;
    int capacity;

    static int get_parent(int pos) {
        return floor((pos - 1.0) / 2);
    }

    int get_smaller_child(int pos) {
        int left_child = 2 * pos + 1, right_child = 2 * pos + 2;
        int smaller_child = -1;

        if (left_child < first_free && right_child < first_free)
            smaller_child = (data[left_child] < data[right_child] ? left_child : right_child);
        else if (left_child < first_free)
            smaller_child = left_child;
        else if (right_child < first_free)
            smaller_child = right_child;

        if (smaller_child != -1 && data[pos] < data[smaller_child])
            smaller_child = -1;

        return smaller_child;
    }

    int get_index(K element) {
        for (int i = 0; i < first_free; i++)
            if (data[i] == element)
                return i;

        return -1;
    }

    void util_up(int pos) {
        while (pos && data[pos] < data[get_parent(pos)]) {
            std::swap(data[pos], data[get_parent(pos)]);
            pos = get_parent(pos);
        }
    }

    void util_down(int pos) {
        int prev = pos;
        while ((pos = get_smaller_child(pos)) != -1) {
            std::swap(data[pos], data[prev]);
            prev = pos;
        }
    }


public:
    priority_queue() {
        first_free = 0;
        capacity = 128;
        data.resize(capacity);
    }

    void push(K element) {
        data[first_free] = element;
        util_up(first_free);

        first_free++;
        if (first_free == capacity) {
            capacity *= 2;
            data.resize(capacity);
        }
    }

    void pop() {
        if (this->empty())
            return;
        if (first_free == 1) {
            first_free = 0;
            return;
        }

        K element = data[first_free - 1];
        data[0] = element;
        first_free--;

        util_down(0);
    }

    void push_pop(K element) {
        if (this->empty())
            return;

        K top = this->top();
        if (top >= element) {
            return;
        }

        data[0] = element;
        util_down(0);
    }

    K top() {
        if (this->empty())
            throw std::logic_error("pq is empty");

        return data[0];
    }

    bool contains(K element) {
        for (int i = 0; i < first_free; i++)
            if (data[i] == element)
                return true;

        return false;
    }

    void erase(K element) {
        int pos = get_index(element);
        if (pos == -1)
            return;

        if (first_free == 1) {
            first_free--;
            return;
        }

        element = data[first_free - 1];
        first_free--;

        data[pos] = element;
        if (pos && data[get_parent(pos)] > data[pos]) {
            util_up(pos);
        }
        else {
            util_down(pos);
        }
    }

    bool empty() {
        return size() == 0;
    }

    int size() {
        return first_free;
    }
};


#endif //DATA_STRUCTURES_PQ_H
