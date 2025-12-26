#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <chrono>
#include <optional>

enum class kMessageType : uint8_t
{
    TYPE0,
    TYPE1,
    TYPE2,
    TYPE3,
    TYPE4,
    TYPE5,
    TYPE6,
    TYPE7,
};

struct MessagePayload{};

struct ProcessingInfo
{
    uint16_t processor_id;
    std::chrono::steady_clock::time_point ts;
};

struct Message
{
    kMessageType type = kMessageType::TYPE0;

    uint16_t producer_id;
    uint16_t seq_number = 0;

    std::chrono::steady_clock::time_point ts = std::chrono::steady_clock::now();

    MessagePayload payload;
};

struct MessageEnvelope
{
    Message msg;
    ProcessingInfo processing_info;
    bool ordering_required = false;
};

#endif