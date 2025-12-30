#include "components/strategy.h"
#include <limits>
#include <emmintrin.h>

constexpr uint64_t INVALID_SEQ = std::numeric_limits<uint64_t>::max();

Strategy::Strategy(InputQ &in, const std::chrono::nanoseconds &processing_time)
    : input_(in), processing_time_(processing_time)
{
    expected_seq_.fill(INVALID_SEQ);
}

void Strategy::ValidateOne()
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + processing_time_;

    auto m_ptr = input_.front();

    while (!m_ptr)
    {
        if(!running_.load(std::memory_order_relaxed))
            return;

        m_ptr = input_.front();
        _mm_pause();
    }

    ++stats.all_passed;

    MessageEnvelope msg(std::move(*m_ptr));

    input_.pop();

    if (msg.ordering_required)
    {
        size_t idx = static_cast<size_t>(msg.msg.type);

        if (expected_seq_[idx] == INVALID_SEQ)
            expected_seq_[idx] = msg.msg.seq_number;

        else if (msg.msg.seq_number != expected_seq_[idx])
            ++stats.violations;

        expected_seq_[idx] = msg.msg.seq_number + 1;
    }

    while (std::chrono::steady_clock::now() < end)
    {
        _mm_pause();
    }
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
