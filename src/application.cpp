#include "config.h"
#include "application.h"
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>

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
                std::make_unique<Producer>(conf, i, stage1_q_));
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
    std::thread timer([this]()
                      { StartExecTimer(); });

    std::vector<std::thread> threads;
    threads.reserve(producers_.size());

    for (auto &producer : producers_)
        threads.emplace_back([p = producer.get()]()
                             { p->Run(); });

    threads.emplace_back([this]()
                         { stage1_r_.Run(); });

    for (auto &processor : processors_)
        threads.emplace_back([p = processor.get()]()
                             { p->Run(); });

    threads.emplace_back([this]()
                         { stage2_r_.Run(); });

    for (auto &strategy : strategies_)
        threads.emplace_back([s = strategy.get()]()
                             { s->Run(); });

    timer.join();

    for (auto &thread : threads)
        thread.join();
}

void Application::StartExecTimer()
{
    auto end = std::chrono::steady_clock::now() + duration_sec_;
    std::this_thread::sleep_until(end);

    StopComponents();
}

void Application::StopComponents()
{
    for (auto &producer : producers_)
        producer->Stop();

    stage1_r_.Stop();

    for (auto &processor : processors_)
        processor->Stop();

    stage2_r_.Stop();

    for (auto &strategy : strategies_)
        strategy->Stop();
}

void Application::ShowStats()
{
    MsgValidatingStats stats;

    for (auto &strategy : strategies_)
    {
        stats.all_passed += strategy->GetStats().all_passed;
        stats.violations += strategy->GetStats().violations;
    }

    std::cout << "all passsed: " << stats.all_passed << std::endl;
    std::cout << "ordering violations: " << stats.violations << std::endl;
}