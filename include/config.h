#ifndef CONFIG_H
#define CONFIG_H

#include <chrono>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr uint8_t MESSAGE_TYPE_COUNT = 8; // 0-7

enum class kScenario
{
    BASELINE,
    HOT_MESSAGE_TYPE,
    BURST_TRAFFIC,
    IMBALANCED_PROCESSING,
    ORDERING_STRESS_TEST,
    STRATEGY_BOTTLENECK
};

inline const std::unordered_map<std::string, kScenario> scenarios{
    {"baseline", kScenario::BASELINE},
    {"hot_message_type", kScenario::HOT_MESSAGE_TYPE},
    {"burst_traffic", kScenario::BURST_TRAFFIC},
    {"imbalanced_processing", kScenario::IMBALANCED_PROCESSING},
    {"ordering_stress_test", kScenario::ORDERING_STRESS_TEST},
    {"strategy_bottleneck", kScenario::STRATEGY_BOTTLENECK},
};

struct StrategyConfig
{
    uint16_t count;
    std::vector<std::chrono::nanoseconds> strategy_processing_time_ns;
};

struct ProducersConfig
{
    uint16_t count;
    uint32_t messages_per_sec;
    std::array<float, MESSAGE_TYPE_COUNT> message_type_distribution;
};

struct Stage1Rule
{
    uint16_t message_type;
    uint16_t processors;
};

struct Stage2Rule
{
    uint16_t message_type;
    uint16_t strategy;
    bool ordering_required;
};

struct ProcessorsConfig
{
    uint16_t count;
    std::array<std::chrono::nanoseconds, MESSAGE_TYPE_COUNT> message_type_processing_time_ns;
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

std::optional<Config> ParseConfig();

uint16_t ParseIndex(const std::string &key, const std::string &prefix);

void FromJson(const json &j, kScenario &scenario);
void FromJson(const json &j, ProducersConfig &producers);
void FromJson(const json &j, ProcessorsConfig &processors);
void FromJson(const json &j, StrategyConfig &strategies);
void FromJson(const json &j, std::vector<Stage1Rule> &rules);
void FromJson(const json &j, std::vector<Stage2Rule> &rules);



#endif