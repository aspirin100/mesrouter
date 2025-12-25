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

    Message msg;

    input_.Pop(msg);

    MessageEnvelope res;

    res.msg = std::move(msg);
    res.processing_info.processor_id = id_;

    output_.Push(std::move(res));

    while (std::chrono::steady_clock::now() < end)
    {
        // _mm_pause();
    }
}
