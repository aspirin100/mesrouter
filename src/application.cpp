#include "config.h"
#include "application.h"
#include <memory>

Application::Application(const Config &conf)
    : stage1_q_(),
      stage2_q_(),
      processors_q_(conf.processors.count),
      strategies_q_(conf.strategies.count),
      stage1_r_(conf, stage1_q_, processors_q_),
      stage2_r_(conf, stage2_q_, strategies_q_),
      duration_sec_(conf.duration_sec)
{
    producers_.reserve(conf.producers.count);

    for (size_t i = 0; i < conf.producers.count; ++i)
        for (size_t j = 0; j < conf.producers.message_type_distribution.size(); ++j)
        {
            if (conf.producers.message_type_distribution[j] == 0)
                continue;

            producers_.emplace_back(
                std::make_unique<Producer>(i,
                                           static_cast<kMessageType>(j),
                                           stage1_q_, conf.producers.messages_per_sec * conf.producers.message_type_distribution[j]));
        }

    processors_.reserve(conf.processors.count);

    for (size_t i = 0; i < conf.processors.count; ++i)
        processors_.emplace_back(
            std::make_unique<Processor>(
                i, processors_q_[i], stage2_q_, conf.processors.message_type_processing_time_ns[i]));

    strategies_.reserve(conf.strategies.count);

    for (size_t i = 0; i < conf.strategies.count; ++i)
        strategies_.emplace_back(
            std::make_unique<Strategy>(strategies_q_[i], conf.strategies.strategy_processing_time_ns[i]));
}

void Application::StartProcess()
{
}
