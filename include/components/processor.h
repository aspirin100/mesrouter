#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "message.h"
#include "../utils/spsc.h"
#include <chrono>
#include <atomic>

class Processor final
{
    using InputQ = spsc_queue<Message>;
    using OutputQ = spsc_queue<MessageEnvelope>;

private:
    std::atomic<bool> running_;

    std::chrono::nanoseconds processing_time_;

    InputQ &input_;
    OutputQ &output_;

    uint16_t id_;

public:
    Processor(const uint16_t id, InputQ &in, OutputQ &out, const std::chrono::nanoseconds &processing_time);

    void Run();
    void Stop();

private:
    void TransformOne();
};

#endif