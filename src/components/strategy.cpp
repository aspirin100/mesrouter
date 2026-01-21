#include "components/strategy.h"
#include <limits>
#include <emmintrin.h>

constexpr uint64_t INVALID_SEQ = std::numeric_limits<uint64_t>::max();

Strategy::Strategy(InputQ &in, const std::chrono::nanoseconds &processing_time)
    : input_(in), processing_time_(processing_time), expected_seq_(0)
{
}

void Strategy::ValidateOne()
{
    auto processing_end = std::chrono::steady_clock::now() + processing_time_;

    auto m_ptr = input_.front();

    while (!m_ptr)
    {
        if (!running_.load(std::memory_order_relaxed))
            return;

        m_ptr = input_.front();
    }

    ++stats.all_passed;

    MessageEnvelope msg(std::move(*m_ptr));

    input_.pop();

    auto &ordering_info = msg.msg.ordering_info;

    if (ordering_info.required)
    {
        if (ordering_info.delivery_seq != expected_seq_)
            ++stats.violations;

        expected_seq_ = ordering_info.delivery_seq + 1;
    }

    while (std::chrono::steady_clock::now() < processing_end)
        _mm_pause();
}

MsgValidatingStats Strategy::GetStats()
{
    return stats;
}

void Strategy::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while (running_.load(std::memory_order_relaxed))
        ValidateOne();
}

void Strategy::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}
