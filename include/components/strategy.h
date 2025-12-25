#ifndef STRATEGY_H
#define STRATEGY_H

#include "message.h"
#include "../utils/mpsc.h"
#include <chrono>
#include <cstdint>

class Strategy
{
    using InputQ = mpsc_queue<MessageEnvelope>;

private:
    std::chrono::nanoseconds processing_time_;
    InputQ &input_;

    std::array<uint64_t, 8> expected_seq_;
    uint16_t violations_ = 0;

public:
    Strategy(InputQ &in, const std::chrono::nanoseconds &processing_time);

    void Run();
    void Stop();

    virtual ~Strategy() = default;

private:
    virtual void ValidateOne();
};

#endif