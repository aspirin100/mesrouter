#ifndef SPSC_H
#define SPSC_H

template <typename T>
class spsc_queue
{
public:
    void Push(T&& val);
    bool TryPop(T& dest);
};

#endif