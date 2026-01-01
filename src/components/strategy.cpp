#include "components/strategy.h"
#include <limits>
#include <emmintrin.h>

constexpr uint64_t INVALID_SEQ = std::numeric_limits<uint64_t>::max();

Strategy::Strategy(uint16_t producers_count, InputQ &in, const std::chrono::nanoseconds &processing_time)
    : input_(in), processing_time_(processing_time)
{
    msg_expected_seq_.resize(producers_count);

    for (auto &seq_arr : msg_expected_seq_)
        seq_arr.fill(INVALID_SEQ);
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

    if (msg.msg.ordering_info.required)
    {
        size_t producer_idx = msg.msg.ordering_info.producer_id;
        size_t msg_idx = static_cast<size_t>(msg.msg.type);

        auto &actual = msg.msg.ordering_info.seq_number;
        auto &expected = msg_expected_seq_[producer_idx][msg_idx];

        if(actual != expected)
        {
            if(expected != INVALID_SEQ)
                ++stats.violations;
        }

        expected = actual + 1;
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
