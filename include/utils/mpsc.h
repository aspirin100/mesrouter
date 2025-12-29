#ifndef MPSC_H
#define MPSC_H

#include <cstddef>
#include <atomic>

template <typename T>
class mpsc_queue
{
private:
    struct Node
    {
        T val;
        std::atomic<Node *> next;
    };

    size_t cap_;

    std::atomic<Node *> head_;
    std::atomic<Node *> tail_;

public:
    explicit mpsc_queue(const size_t capacity = 1024);

    void Push(T &&val);
    void Pop(T &dest);
};

template <typename T>
mpsc_queue<T>::mpsc_queue(const size_t capacity) : cap_(capacity) {}

template <typename T>
void mpsc_queue<T>::Push(T &&val)
{
}

template <typename T>
void mpsc_queue<T>::Pop(T &dest) {}

#endif