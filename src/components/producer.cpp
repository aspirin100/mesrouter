#include "components/producer.h"
#include "components/message.h"
#include <utility>

Producer::Producer(const uint64_t producer_id, const kMessageType msg_type, OutputQ &out)
    : id_(producer_id), producing_msg_type_(msg_type), output_(out)
{
}

void Producer::ProduceMessage()
{

    Message msg;

    msg.producer_id = id_;
    msg.seq_number = seq_;
    msg.type = producing_msg_type_;

    ++seq_;

    output_.Push(std::move(msg));
}

void Producer::Run()
{
    running_.store(true);

    while(running_.load(std::memory_order_relaxed))
        ProduceMessage();
}

void Producer::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}