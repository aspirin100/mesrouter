#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "message.h"
#include "../utils/spsc.h"
#include <chrono>

class Processor final
{
    using InputQ = spsc_queue<Message>;
    using OutputQ = spsc_queue<MessageEnvelope>;

private:
    std::chrono::nanoseconds processing_time_;

    InputQ &input_;
    OutputQ &output_;

    uint16_t id_;

public:
    Processor(const uint16_t id, InputQ &in, OutputQ &out, const std::chrono::nanoseconds &processing_time);

    void Start();
    void Stop();

private:
    void TransformOne();
};

#endif