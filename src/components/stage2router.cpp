#include "components/router.h"
#include <utility>
#include <emmintrin.h>

Stage2Router::Stage2Router(const Config &conf, InputQ &in, std::vector<OutputQ> &out)
    : input_(in), output_(out),
      msg_type_output_(conf.stage_2_rules.size(), INVALID_OUTPUT)
{
    sequencer_.msg_type_ordering_requirement.resize(conf.stage_2_rules.size());

    sequencer_.expected_seq.resize(conf.producers.count);
    for (auto &msg_type_seq : sequencer_.expected_seq)
        msg_type_seq.fill(0);

    for (const auto &rule : conf.stage_2_rules)
    {
        if (rule.strategy >= out.size())
            throw std::logic_error("unknown strategy");
        if (rule.message_type >= msg_type_output_.size())
            throw std::logic_error("unknown message type");
        if (msg_type_output_[rule.message_type] != INVALID_OUTPUT)
            throw std::logic_error("message type duplcate strategy setting");

        msg_type_output_[rule.message_type] = rule.strategy;

        sequencer_.msg_type_ordering_requirement[rule.message_type] = rule.ordering_required;
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

    if (msg.msg.ordering_info.required = CheckOrderingRequired(msg.msg); !msg.msg.ordering_info.required)
        SendOne(msg);

    // TODO: potentially buffer type change. Implement re-sequencing
    auto &ordering_info = msg.msg.ordering_info;

    auto producer_idx = ordering_info.producer_id;
    auto msg_type_idx = static_cast<uint8_t>(msg.msg.type);

    auto &expected_seq = sequencer_.expected_seq[producer_idx][msg_type_idx];
    auto &actual_seq = ordering_info.producer_seq;

    if (actual_seq != expected_seq)
    {
        sequencer_.buffer[producer_idx][msg_type_idx] = std::move(msg);
        return;
    }

    msg.msg.ordering_info.delivery_seq = sequencer_.next_seq++;
}

void Stage2Router::SendOne(MessageEnvelope &msg)
{
    uint16_t producer_idx = SelectOutput(msg.msg);

    assert(producer_idx != INVALID_OUTPUT);
    assert(producer_idx < output_.size());

    while (!output_[producer_idx].try_emplace(msg))
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
    return sequencer_.msg_type_ordering_requirement[static_cast<uint8_t>(msg.type)];
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