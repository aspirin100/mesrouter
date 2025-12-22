#ifndef PRODUCER_H
#define PRODUCER_H

#include "message.h"
#include <chrono>

class Producer
{
private:
    std::chrono::milliseconds producing_time_;

public:
    Producer(const std::chrono::milliseconds &producing_time) : producing_time_(producing_time) {}

    virtual Message ProduceMessage() const;

    virtual ~Producer() = default;
};

#endif