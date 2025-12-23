#ifndef PRODUCER_H
#define PRODUCER_H

#include "message.h"
#include <cstdint>
#include <atomic>

class Producer // TODO: add stage1router que
{
private:
    uint64_t seq_ = 0;
    kMessageType producing_msg_type_;
public:
    Producer(const uint64_t producer_id, const kMessageType msg_type)
         : id(producer_id), producing_msg_type_(msg_type) {}

    virtual Message ProduceMessage();

    virtual ~Producer() = default;

    uint64_t id;
};

#endif