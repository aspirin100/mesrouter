#ifndef STRATEGY_H
#define STRATEGY_H

#include "../utils/spsc.h"
#include "../config.h"
#include "message.h"
#include <chrono>
#include <cstdint>
#include <atomic>

struct MsgValidatingStats
{
    uint64_t violations = 0;
    uint64_t all_passed = 0;
};

class Strategy
{
    using InputQ = rigtorp::SPSCQueue<MessageEnvelope>;

private:
    std::atomic<bool> running_;

    InputQ &input_;
    std::chrono::nanoseconds processing_time_;

    std::vector<std::array<uint64_t, MESSAGE_TYPE_COUNT>> msg_expected_seq_;

    MsgValidatingStats stats;

public:
    Strategy(uint16_t producers_count, InputQ &in, const std::chrono::nanoseconds &processing_time);

    void Run();
    void Stop();

    MsgValidatingStats GetStats();

    virtual ~Strategy() = default;

private:
    virtual void ValidateOne();
};

#endif