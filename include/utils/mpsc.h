#ifndef MPSC_H
#define MPSC_H

template <typename T>
class mpsc_queue 
{
private:
    size_t cap_;
public:
    explicit mpsc_queue(const size_t capacity);

    void Push(T &&val);
    void Pop(T &dest);
};

template <typename T>
mpsc_queue<T>::mpsc_queue(const size_t capacity) : cap_(capacity) {}



#endif