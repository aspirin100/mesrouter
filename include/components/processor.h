#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "message.h"
#include <chrono>

class Processor
{
private:
    std::chrono::nanoseconds processing_time_;

public:
    Processor(const std::chrono::nanoseconds &processing_time) : processing_time_(processing_time_) {}

    virtual void TransformMessage(Message &msg);

    virtual ~Processor() = default;
};

#endif