#include "components/processor.h"
#include "components/message.h"
#include <chrono>
#include <emmintrin.h>

Processor::Processor(const uint16_t id, InputQ &in, OutputQ &out, const std::chrono::nanoseconds &processing_time)
    : id_(id), input_(in), output_(out), processing_time_(processing_time)
{
}

void Processor::TransformOne()
{
    auto processing_end = std::chrono::steady_clock::now() + processing_time_;

    Message *msg_ptr;

    while (true)
    {
        if (!running_.load(std::memory_order_relaxed))
            return;

        if (msg_ptr = input_.front(); msg_ptr)
            break;
        else
            _mm_pause();
    }

    MessageEnvelope res;

    res.processing_info.processor_id = id_;
    res.msg = std::move(*msg_ptr);

    input_.pop();

    while (!output_.try_emplace(res))
    {
        if (!running_.load(std::memory_order_relaxed))
            return;
        else
            _mm_pause();
    }

    while (std::chrono::steady_clock::now() < processing_end)
        _mm_pause();
    
}

void Processor::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while (running_.load(std::memory_order_relaxed))
        TransformOne();
}

void Processor::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}