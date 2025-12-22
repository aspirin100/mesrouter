#ifndef MPSC_H
#define MPSC_H

template <typename T>
class mpsc_queue
{
public:
    bool TryPop(T& dest);
};


#endif