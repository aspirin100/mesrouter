#include "components/producer.h"
#include "components/message.h"
#include <utility>
#include <emmintrin.h>

Producer::Producer(const Config &conf, uint64_t producer_id, OutputQ &out) // TODO: config validation(sum of msg distribution must be 1)
    : id_(producer_id),
      output_(out),
      limiter_(conf.producers.messages_per_sec)
{
    float acc = 0;
    for (uint8_t i = 0; i < conf.producers.message_type_distribution.size(); ++i)
    {
        acc += conf.producers.message_type_distribution[i];
        chooser.cdf[i] = acc;
    }
}

void Producer::ProduceMessage()
{
    Message msg;

    msg.producer_id = id_;
    msg.seq_number = seq_;
    msg.type = SelectMsgType();

    ++seq_;

    while(!output_.try_emplace(msg))
    {
        if(!running_.load(std::memory_order_relaxed))
            return;
        else
            _mm_pause();
    }
}

kMessageType Producer::SelectMsgType()
{
    float x = chooser.range(chooser.rnd);

    for (size_t i = 0; i < MESSAGE_TYPE_COUNT; ++i)
    {
        if (x < chooser.cdf[i])
            return static_cast<kMessageType>(i);
    }

    return static_cast<kMessageType>(MESSAGE_TYPE_COUNT - 1);
}

void Producer::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while (running_.load(std::memory_order_relaxed))
    {
       if (limiter_.can_produce())
            ProduceMessage();
       else
          limiter_.wait_for_next_batch();
    }
}

void Producer::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}