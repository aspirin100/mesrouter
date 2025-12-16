#ifndef CONFIG_H
#define CONFIG_H

#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class kScenario
{
    BASELINE,
    HOT_MESSAGE_TYPE,
    BURST_TRAFFIC,
    IMBALANCED_PROCESSING,
    ORDERING_STRESS_TEST,
    STRATEGY_BOTTLENECK
};

enum class kMessageType
{
    TYPE0,
    TYPE1,
    TYPE2,
    TYPE3
};

enum class kStrategyType
{
    STRATEGY0,
    STRATEGY1,
    STRATEGY2
};

struct StrategyConfig
{
    uint16_t count;
    std::unordered_map<kStrategyType, std::chrono::nanoseconds> processing_time_ns;
};

struct ProducersConfig
{
    uint16_t count;
    uint32_t messages_per_sec;
    std::unordered_map<kMessageType, float> message_distribution;
};

struct Stage1Rule
{
    kMessageType message_type;
    uint16_t processors;
};

struct Stage2Rule
{
    kMessageType message_type;
    kStrategyType strategy;
    bool ordering_required;
};

struct ProcessorsConfig
{
    uint16_t count;
    std::unordered_map<kMessageType, std::chrono::nanoseconds> processing_time_ns;
};

struct Config
{
    kScenario scenario;
    std::chrono::duration<float> duration_sec;
    ProducersConfig producers;
    ProcessorsConfig processors;
    StrategyConfig strategies;
    std::vector<Stage1Rule> stage_1_rules;
    std::vector<Stage2Rule> stage_2_rules;
};

kScenario ParseScenario(const std::string &scenario);
ProducersConfig ParseProducersConfig(const json &data);
ProcessorsConfig ParseProcessorsConfig(const json &data);
StrategyConfig ParseStrategies(const json &data);
std::vector<Stage1Rule> ParseStage1Rules(const json &data);
std::vector<Stage2Rule> ParseStage2Rules(const json &data);
kMessageType ParseMessageType(const int type);
kStrategyType ParseStrategyType(const int type);
std::optional<Config> ParseConfig();


#endif