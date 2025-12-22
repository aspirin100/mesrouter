#ifndef APPLICATION_H
#define APPLICATION_H

#include "config.h"
#include "components/producer.h"
#include "components/processor.h"
#include "components/strategy.h"
#include "components/message.h"
#include <vector>

class Application
{
private:
    std::vector<Producer> producers_;
    std::vector<Processor> processors_;
    std::vector<Strategy> strategies_;

    uint32_t duration_sec_;

public:
    Application(const Config& conf);

    void StartProcess();
};


#endif