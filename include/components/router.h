#ifndef ROUTER_H
#define ROUTER_H

#include "../utils/mpsc.h"
#include "../utils/spsc.h"
#include "../config.h"
#include "message.h"
#include "processor.h"
#include <array>
#include <vector>

class Stage1Router
{
    using InputQ = mpsc_queue<Message>;
    using OutputQ = spsc_queue<Message>;

private:
    InputQ input_;
    std::array<size_t, 8> msg_type_output_;
    std::vector<OutputQ *> output_;

public:
    Stage1Router(const Config &conf, std::vector<OutputQ *> out);

    void Run();
    void Stop();

private:
    void RouteOne();
    size_t SelectOutput(const Message &msg) const noexcept;
};

class Stage2Router
{
};

#endif