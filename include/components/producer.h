#ifndef PRODUCER_H
#define PRODUCER_H

#include "message.h"
#include "../utils/mpmc.h"
#include "../utils/limiter.h"
#include <cstdint>
#include <atomic>

class Producer
{
    using OutputQ = rigtorp::mpmc::Queue<Message>;

private:
    std::atomic<bool> running_ = false;

    uint64_t id_;
    uint64_t seq_ = 0;
    kMessageType producing_msg_type_;

    OutputQ &output_;

    BatchRateLimiter limiter_;

public:
    Producer(uint64_t producer_id, kMessageType msg_type, OutputQ &out, uint32_t msg_per_sec);

    void Run();
    void Stop();

    virtual ~Producer() = default;

protected:
    virtual void ProduceMessage();
};

#endif