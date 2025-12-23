#include "components/processor.h"
#include "components/message.h"
#include <chrono>

void Processor::TransformOne()
{
    auto start = std::chrono::steady_clock::now();
    
    Message msg;

    if(input_.TryPop(msg))
    {
        MessageEnvelope res;

        res.msg = std::move(msg);
        res.processing_info.processor_id = id_;

        output_.Push(std::move(res));
    }

    while(std::chrono::steady_clock::now() - start < processing_time_)
    {
        // _mm_pause();
    }
}

