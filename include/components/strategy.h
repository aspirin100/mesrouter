#ifndef STRATEGY_H
#define STRATEGY_H

#include "message.h"
#include <chrono>

class Strategy
{
private:
    std::chrono::nanoseconds processing_time_;

public:
    Strategy(const std::chrono::nanoseconds &processing_time) : processing_time_(processing_time_) {}

    virtual void ValidateOrdering(const Message &msg) const;

    virtual ~Strategy() = default;
};

#endif