#include "config.h"

#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <fstream>
#include <chrono>
#include <utility>
#include <iostream>

using json = nlohmann::json;

std::optional<Config> ParseConfig()
{
    char *config_path = std::getenv("CONFIG_PATH");
    if (!config_path)
    {
        std::cout << config_path << std::endl;
        return std::nullopt;
    }

    std::ifstream conf(config_path);
    if (!conf)
    {
        std::cout << "failed to open conf file" << std::endl;
        return std::nullopt;
    }

    json data = json::parse(conf);

    Config config;

    config.scenario = ParseScenario(data["scenario"]);
    config.duration_sec = std::chrono::duration<float>(data["duration_secs"]);

    config.producers = ParseProducersConfig(data);
    config.processors = ParseProcessorsConfig(data);
    config.strategies = ParseStrategies(data);
    config.stage_1_rules = ParseStage1Rules(data);
    config.stage_2_rules = ParseStage2Rules(data);

    return config;
}

kScenario ParseScenario(const std::string &scenario)
{
    if (scenario == "baseline")
        return kScenario::BASELINE;
    else if (scenario == "Hot Message Type")
        return kScenario::HOT_MESSAGE_TYPE;
    else if (scenario == "Burst Traffic")
        return kScenario::BURST_TRAFFIC;
    else if (scenario == "Imbalanced Processing")
        return kScenario::IMBALANCED_PROCESSING;
    else if (scenario == "Ordering Stress Test")
        return kScenario::ORDERING_STRESS_TEST;
    else if (scenario == "Strategy Bottleneck")
        return kScenario::STRATEGY_BOTTLENECK;

    return kScenario::BASELINE;
}

ProducersConfig ParseProducersConfig(const json &data)
{
    ProducersConfig conf;
    conf.count = data["producers"]["count"];
    conf.messages_per_sec = data["producers"]["messages_per_sec"];

    conf.message_distribution[kMessageType::TYPE0] = data["producers"]["distribution"]["msg_type_0"];
    conf.message_distribution[kMessageType::TYPE1] = data["producers"]["distribution"]["msg_type_1"];
    conf.message_distribution[kMessageType::TYPE2] = data["producers"]["distribution"]["msg_type_2"];
    conf.message_distribution[kMessageType::TYPE3] = data["producers"]["distribution"]["msg_type_3"];

    return conf;
}

ProcessorsConfig ParseProcessorsConfig(const json &data)
{
    ProcessorsConfig conf;
    conf.count = data["processors"]["count"];

    conf.processing_time_ns[kMessageType::TYPE0] = std::chrono::nanoseconds(data["processors"]["processing_times_ns"]["msg_type_0"]);
    conf.processing_time_ns[kMessageType::TYPE1] = std::chrono::nanoseconds(data["processors"]["processing_times_ns"]["msg_type_1"]);
    conf.processing_time_ns[kMessageType::TYPE2] = std::chrono::nanoseconds(data["processors"]["processing_times_ns"]["msg_type_2"]);
    conf.processing_time_ns[kMessageType::TYPE3] = std::chrono::nanoseconds(data["processors"]["processing_times_ns"]["msg_type_3"]);

    return conf;
}

StrategyConfig ParseStrategies(const json &data)
{
    StrategyConfig conf;
    conf.count = data["strategies"]["count"];

    conf.processing_time_ns[kStrategyType::STRATEGY0] = std::chrono::nanoseconds(data["strategies"]["processing_times_ns"]["strategy_0"]);
    conf.processing_time_ns[kStrategyType::STRATEGY1] = std::chrono::nanoseconds(data["strategies"]["processing_times_ns"]["strategy_1"]);
    conf.processing_time_ns[kStrategyType::STRATEGY2] = std::chrono::nanoseconds(data["strategies"]["processing_times_ns"]["strategy_2"]);

    return conf;
}

std::vector<Stage1Rule> ParseStage1Rules(const json &data)
{
    std::vector<Stage1Rule> rules;

    for (const auto &rule : data["stage1_rules"])
    {
        Stage1Rule temp;

        temp.message_type = ParseMessageType(rule["msg_type"]);
        temp.processors = rule["processors"];

        rules.emplace_back(std::move(temp));
    }

    return rules;
}

kMessageType ParseMessageType(const int type)
{
    switch (type)
    {
    case 0:
        return kMessageType::TYPE0;
        break;
    case 1:
        return kMessageType::TYPE1;
        break;
    case 2:
        return kMessageType::TYPE2;
        break;
    case 3:
        return kMessageType::TYPE3;
        break;
    default:
        return kMessageType::TYPE0;
    }
}

std::vector<Stage2Rule> ParseStage2Rules(const json &data)
{
    std::vector<Stage2Rule> rules;

    for (const auto &rule : data["stage2_rules"])
    {
        Stage2Rule temp;

        temp.message_type = ParseMessageType(rule["msg_type"]);
        temp.strategy = ParseStrategyType(rule["strategy"]);
        temp.ordering_required = rule["ordering_required"];

        rules.emplace_back(std::move(temp));
    }

    return rules;
}

kStrategyType ParseStrategyType(const int type)
{
    switch (type)
    {
    case 0:
        return kStrategyType::STRATEGY0;
        break;
    case 1:
        return kStrategyType::STRATEGY1;
        break;
    case 2:
        return kStrategyType::STRATEGY2;
        break;
    default:
        return kStrategyType::STRATEGY0;
    }
}
