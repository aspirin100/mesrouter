#ifndef ROUTER_H
#define ROUTER_H

#include "../utils/mpmc.h"
#include "../utils/spsc.h"
#include "../config.h"
#include "message.h"
#include "processor.h"
#include <array>
#include <vector>
#include <atomic>

constexpr uint16_t INVALID_OUTPUT = std::numeric_limits<uint16_t>::max();

class Stage1Router
{
    using InputQ = rigtorp::mpmc::Queue<Message>;
    using OutputQ = rigtorp::SPSCQueue<Message>;

private:
    std::atomic<bool> running_;

    InputQ &input_;
    std::array<uint16_t, MESSAGE_TYPE_COUNT> msg_type_output_;
    std::vector<OutputQ> &output_;

public:
    Stage1Router(const Config &conf, InputQ &in, std::vector<OutputQ> &out);

    void Run();
    void Stop();

private:
    void RouteOne();
    size_t SelectOutput(const Message &msg) const noexcept;
};

class Stage2Router
{
    using InputQ = rigtorp::mpmc::Queue<MessageEnvelope>;
    using OutputQ = rigtorp::SPSCQueue<MessageEnvelope>;

private:

    struct Sequencer
    {
        uint64_t next_seq = 0;
        std::vector<std::array<uint64_t, MESSAGE_TYPE_COUNT>> expected_seq;
        std::vector<std::array<MessageEnvelope, MESSAGE_TYPE_COUNT>> buffer;
        std::vector<bool> msg_type_ordering_requirement;
    };

private:
    std::atomic<bool> running_;

    InputQ &input_;
    std::vector<OutputQ> &output_;

    std::vector<uint16_t> msg_type_output_;
    Sequencer sequencer_;

public:
    Stage2Router(const Config &conf, InputQ &in, std::vector<OutputQ> &out);

    void Run();
    void Stop();

private:
    void RouteOne();
    void SendOne(MessageEnvelope &msg);
    size_t SelectOutput(const Message &msg);
    bool CheckOrderingRequired(const Message &msg);
};

#endif