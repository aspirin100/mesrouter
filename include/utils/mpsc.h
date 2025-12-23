#ifndef MPSC_H
#define MPSC_H

template <typename T>
class mpsc_queue
{
public:
    void Push(T &&val);
    void Pop(T &dest);
};

#endif