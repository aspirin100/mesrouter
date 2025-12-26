#ifndef STRATEGY_H
#define STRATEGY_H

#include "../utils/mpsc.h"
#include "../config.h"
#include "message.h"
#include <chrono>
#include <cstdint>
#include <atomic>

class Strategy
{
    using InputQ = mpsc_queue<MessageEnvelope>;

private:
    std::atomic<bool> running_;

    InputQ &input_;
    std::chrono::nanoseconds processing_time_;

    std::array<uint64_t, MESSAGE_TYPE_COUNT> expected_seq_;
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