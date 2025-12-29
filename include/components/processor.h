#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "message.h"
#include "../utils/spsc.h"
#include "../utils/mpmc.h"
#include <chrono>
#include <atomic>

class Processor final
{
    using InputQ = rigtorp::SPSCQueue<Message>;
    using OutputQ = rigtorp::mpmc::Queue<MessageEnvelope>;

private:
    uint16_t id_;

    InputQ &input_;
    OutputQ &output_;

    std::chrono::nanoseconds processing_time_;
    
    std::atomic<bool> running_;

public:
    Processor(const uint16_t id, InputQ &in, OutputQ &out, const std::chrono::nanoseconds &processing_time);

    void Run();
    void Stop();

private:
    void TransformOne();
};

#endif