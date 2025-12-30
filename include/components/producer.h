#ifndef PRODUCER_H
#define PRODUCER_H

#include "message.h"
#include "../config.h"
#include "../utils/mpmc.h"
#include "../utils/limiter.h"
#include <cstdint>
#include <atomic>
#include <random>

class Producer
{
    using OutputQ = rigtorp::mpmc::Queue<Message>;

private:
    struct MessageTypeChooser
    {
        std::array<float, MESSAGE_TYPE_COUNT> cdf;
        std::mt19937_64 rnd;
        std::uniform_real_distribution<float> range{0.0f, 1.0f};
    };

private:
    std::atomic<bool> running_ = false;

    uint64_t id_;
    std::array<uint64_t, MESSAGE_TYPE_COUNT> msg_type_seq_;

    OutputQ &output_;

    BatchRateLimiter limiter_;
    MessageTypeChooser chooser;

public:
    Producer(const Config &conf, uint64_t producer_id, OutputQ &out);

    void Run();
    void Stop();

    virtual ~Producer() = default;

protected:
    virtual void ProduceMessage();

private:
    kMessageType SelectMsgType();
};

#endif