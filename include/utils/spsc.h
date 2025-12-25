#ifndef SPSC_H
#define SPSC_H

template <typename T>
class spsc_queue
{
private:
    size_t cap_;

public:
    mpsc_queue(const size_t capacity);

    void Push(T &&val);
    void Pop(T &dest);
};

#endif