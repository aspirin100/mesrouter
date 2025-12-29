#ifndef APPLICATION_H
#define APPLICATION_H

#include "config.h"
#include "components/producer.h"
#include "components/processor.h"
#include "components/strategy.h"
#include "components/message.h"
#include "components/router.h"
#include "utils/mpmc.h"
#include "utils/spsc.h"
#include <vector>
#include <chrono>
#include <memory>

class Application
{
private:
    rigtorp::mpmc::Queue<Message> stage1_q_;
    rigtorp::mpmc::Queue<MessageEnvelope> stage2_q_;
    std::vector<rigtorp::SPSCQueue<Message>> processors_q_;
    std::vector<rigtorp::SPSCQueue<MessageEnvelope>> strategies_q_;

    Stage1Router stage1_r_;
    Stage2Router stage2_r_;

    std::vector<std::unique_ptr<Producer>> producers_;
    std::vector<std::unique_ptr<Processor>> processors_;
    std::vector<std::unique_ptr<Strategy>> strategies_;

    std::chrono::duration<float> duration_sec_;

public:
    Application(const Config &conf);

    void StartProcess();
    void ShowStats();

private:
    void StartExecTimer();
    void StopComponents();
};

#endif