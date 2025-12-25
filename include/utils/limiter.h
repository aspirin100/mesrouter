#ifndef LIMITER_H
#define LIMITER_H

#include <cstdint>
#include <chrono>

class BatchRateLimiter
{
    constexpr static uint32_t NS_PER_SEC = 1'000'000'000;
    uint32_t batch_size_;
    uint64_t interval_ns_;
    std::chrono::steady_clock::time_point batch_start_;
    uint32_t produced_in_batch_ = 0;

public:
    BatchRateLimiter(uint32_t rate_per_second, uint32_t batch_size = 1024)
        : batch_size_(batch_size), interval_ns_(NS_PER_SEC * batch_size / rate_per_second), batch_start_(std::chrono::steady_clock::now()) {}

    bool can_produce()
    {
        if (produced_in_batch_ % 64 == 0)
        { 
            auto now = std::chrono::steady_clock::now();
            if (now - batch_start_ >= std::chrono::nanoseconds(interval_ns_))
            {
                batch_start_ = now;
                produced_in_batch_ = 0;
            }
        }

        if (produced_in_batch_ < batch_size_)
        {
            ++produced_in_batch_;
            return true;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - batch_start_;

        if (elapsed >= std::chrono::nanoseconds(interval_ns_))
        {
            batch_start_ = now;
            produced_in_batch_ = 1;
            return true;
        }

        return false;
    }

    void wait_for_next_batch()
    {
        auto deadline = batch_start_ + std::chrono::nanoseconds(interval_ns_);
        while (std::chrono::steady_clock::now() < deadline)
        {
            // _mm_pause();
        }
        batch_start_ = std::chrono::steady_clock::now();
        produced_in_batch_ = 0;
    }
};

#endif