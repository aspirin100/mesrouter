#include "components/producer.h"
#include "components/message.h"
#include <utility>

Producer::Producer(uint64_t producer_id, kMessageType msg_type, OutputQ &out, uint32_t msg_per_sec)
    : id_(producer_id), producing_msg_type_(msg_type), output_(out), limiter_(msg_per_sec)
{
}

void Producer::ProduceMessage()
{

    Message msg;

    msg.producer_id = id_;
    msg.seq_number = seq_;
    msg.type = producing_msg_type_;

    ++seq_;

    output_.push(std::move(msg));
}

void Producer::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while(running_.load(std::memory_order_relaxed))
    {
        if(limiter_.can_produce())
            ProduceMessage();
        else
            limiter_.wait_for_next_batch();
    }
}

void Producer::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}