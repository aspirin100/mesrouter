#include "components/router.h"
#include <utility>
#include <limits>
#include <stdexcept>
#include <vector>

Stage1Router::Stage1Router(const Config &conf, InputQ &in, std::vector<OutputQ> &out)
    : input_(in), output_(out)
{
    msg_type_output_.fill(INVALID_OUTPUT);

    for (const auto &rule : conf.stage_1_rules)
    {
        if (rule.message_type >= msg_type_output_.size())
            throw std::logic_error("invalid message type");

        if (rule.processors >= output_.size())
            throw std::logic_error("invalid processor id");

        if (msg_type_output_[rule.message_type] != INVALID_OUTPUT)
            throw std::logic_error("duplicate processor settings for message type");

        msg_type_output_[rule.message_type] = rule.processors;
    }
}

size_t Stage1Router::SelectOutput(const Message &msg) const noexcept
{
    return msg_type_output_[static_cast<size_t>(msg.type)];
}

void Stage1Router::RouteOne()
{
    Message msg;

    while (!input_.try_pop(msg))
        if (!running_.load(std::memory_order_relaxed))
            return;

    size_t idx = SelectOutput(msg);

    assert(idx != INVALID_OUTPUT);
    assert(idx < output_.size());

    while (!output_[idx].try_emplace(msg))
        if (!running_.load(std::memory_order_relaxed))
            return;
}

void Stage1Router::Run()
{
    running_.store(true, std::memory_order_relaxed);

    while (running_.load(std::memory_order_relaxed))
        RouteOne();
}

void Stage1Router::Stop()
{
    running_.store(false, std::memory_order_relaxed);
}