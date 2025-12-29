#include "components/processor.h"
#include "components/message.h"
#include <chrono>

Processor::Processor(const uint16_t id, InputQ &in, OutputQ &out, const std::chrono::nanoseconds &processing_time)
    : id_(id), input_(in), output_(out), processing_time_(processing_time)
{
}

void Processor::TransformOne()
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + processing_time_;

    auto m_ptr = input_.front();

    while (!m_ptr)
    {
        // _mm_pause();
        m_ptr = input_.front();
    }

    MessageEnvelope res;

    res.processing_info.processor_id = id_;
    res.msg = std::move(*m_ptr);

    input_.pop();

    output_.Push(std::move(res));

    while (std::chrono::steady_clock::now() < end)
    {
        // _mm_pause();
    }
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