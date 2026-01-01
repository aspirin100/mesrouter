#include "components/router.h"
#include <utility>
#include <emmintrin.h>

Stage2Router::Stage2Router(const Config &conf, InputQ &in, std::vector<OutputQ> &out)
    : input_(in), output_(out),
      msg_type_output_(conf.stage_2_rules.size(), INVALID_OUTPUT),
      msg_type_ordering_requirement_(conf.stage_2_rules.size(), false)
{
    for (const auto &rule : conf.stage_2_rules)
    {
        if (rule.strategy >= out.size())
            throw std::logic_error("unknown strategy");
        if (rule.message_type >= msg_type_output_.size())
            throw std::logic_error("unknown message type");
        if (msg_type_output_[rule.message_type] != INVALID_OUTPUT)
            throw std::logic_error("message type duplcate strategy setting");

        msg_type_output_[rule.message_type] = rule.strategy;
        msg_type_ordering_requirement_[rule.message_type] = rule.ordering_required; // maybe join into one std::vector<> with help struct
    }
}

void Stage2Router::RouteOne()
{
    MessageEnvelope msg;

    while (!input_.try_pop(msg))
    {
        if (!running_.load(std::memory_order_relaxed))
            return;
        else
            _mm_pause();
    }

    uint16_t idx = SelectOutput(msg.msg);

    assert(idx != INVALID_OUTPUT);
    assert(idx < output_.size());

    msg.msg.ordering_info.required = CheckOrderingRequired(msg.msg);

    while (!output_[idx].try_emplace(msg))
    {
        if (!running_.load(std::memory_order_relaxed))
            return;
        else
            _mm_pause();
    }
}

size_t Stage2Router::SelectOutput(const Message &msg)
{
    return msg_type_output_[static_cast<uint8_t>(msg.type)];
}

bool Stage2Router::CheckOrderingRequired(const Message &msg)
{
    return msg_type_ordering_requirement_[static_cast<uint8_t>(msg.type)];
}

void Stage2Router::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while (running_.load(std::memory_order_relaxed))
        RouteOne();
}

void Stage2Router::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}