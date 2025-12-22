#include "components/router.h"
#include <utility>
#include <limits>
#include <stdexcept>
#include <vector>

constexpr size_t INVALID_OUTPUT = std::numeric_limits<size_t>::max();

Stage1Router::Stage1Router(const Config &conf, std::vector<OutputQ *> out)
    : output_(out)
{
    msg_type_output_.fill(INVALID_OUTPUT);

    for (const auto &rule : conf.stage_1_rules)
    {
        if (rule.message_type >= msg_type_output_.size())
            throw std::logic_error("invalid message type");

        if (rule.processors >= output_.size())
            throw std::logic_error("invalid processor id");

        if (msg_type_output_[rule.message_type] != INVALID_OUTPUT)
            throw std::logic_error("duplicate settings for message type");

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

    if(input_.TryPop(msg))
    {
        size_t idx = SelectOutput(msg);

        assert(idx != INVALID_OUTPUT);
        assert(output_[idx] != nullptr);

        output_[idx]->Push(std::move(msg));
    }
}
