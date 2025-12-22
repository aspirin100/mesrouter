#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <chrono>

class Message
{
using Timestamp = std::chrono::time_point<std::chrono::system_clock>; // TODO: maybe use other type if need 

private:
    uint8_t type_; // 0-7

    uint16_t producer_id_;
    uint16_t processor_id_;
    uint16_t seq_number_ = 0;
    
    Timestamp produced_ts_ = std::chrono::system_clock::now();
    Timestamp processed_ts_;

public:
    Message(const uint8_t type, const uint16_t producer_id);

    void Process();

    virtual ~Message() = default;
};

#endif