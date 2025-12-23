#ifndef PRODUCER_H
#define PRODUCER_H

#include "message.h"
#include "../utils/mpsc.h"
#include <cstdint>
#include <atomic>

class Producer
{
    using OutputQ = mpsc_queue<Message>;

private:
    uint64_t id_;
    uint64_t seq_ = 0;
    kMessageType producing_msg_type_;

    OutputQ &output_;

public:
    Producer(const uint64_t producer_id, const kMessageType msg_type, OutputQ &out);

    void Run();
    void Stop();

    virtual ~Producer() = default;

protected:
    virtual void ProduceMessage();
};

#endif