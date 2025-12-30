#include "components/strategy.h"
#include <limits>
#include <emmintrin.h>

Strategy::Strategy(uint16_t producers_count, InputQ &in, const std::chrono::nanoseconds &processing_time)
    : input_(in), processing_time_(processing_time)
{
    msg_expected_seq_.resize(producers_count);

    for (auto &seq_arr : msg_expected_seq_)
        seq_arr.fill(0);
}

void Strategy::ValidateOne()
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + processing_time_;

    auto m_ptr = input_.front();

    while (!m_ptr)
    {
        if (!running_.load(std::memory_order_relaxed))
            return;

        m_ptr = input_.front();
        _mm_pause();
    }

    ++stats.all_passed;

    MessageEnvelope msg(std::move(*m_ptr));

    input_.pop();

    if (msg.msg.ordering_info.required)
    {
        size_t producer_idx = msg.msg.ordering_info.producer_id;
        size_t msg_idx = static_cast<size_t>(msg.msg.type);

        if (msg.msg.ordering_info.seq_number != msg_expected_seq_[producer_idx][msg_idx])
        {
            ++stats.violations;
            msg_expected_seq_[producer_idx][msg_idx] = msg.msg.ordering_info.seq_number; 
        }
        
        ++msg_expected_seq_[producer_idx][msg_idx];
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
