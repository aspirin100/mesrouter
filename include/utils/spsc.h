#ifndef SPSC_H
#define SPSC_H

#include <cstddef>

template <typename T>
class spsc_queue
{
private:
    size_t cap_;

public:
    spsc_queue(const size_t capacity = 1024);

    void Push(T &&val);
    void Pop(T &dest);
};

template <typename T>
spsc_queue<T>::spsc_queue(const size_t capacity) {}

template <typename T>
void spsc_queue<T>::Push(T &&val) {}

template <typename T>
void spsc_queue<T>::Pop(T &dest) {}

#endif