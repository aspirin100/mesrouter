#include "config.h"

#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <fstream>
#include <chrono>
#include <utility>
#include <iostream>
#include <stdexcept>

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

    FromJson(data, config.scenario);
    config.duration_sec = std::chrono::duration<float>(data.at("duration_secs"));
    FromJson(data, config.producers);
    FromJson(data, config.processors);
    FromJson(data, config.strategies);
    FromJson(data, config.stage_1_rules);
    FromJson(data, config.stage_2_rules);

    return config;
}

void FromJson(const json &j, kScenario &scenario)
{
    auto it = scenarios.find(j.at("scenario"));
    if (it != scenarios.end())
        scenario = it->second;
}

void FromJson(const json &j, ProducersConfig &producers)
{
    j.at("producers").at("count").get_to(producers.count);
    j.at("producers").at("messages_per_sec").get_to(producers.messages_per_sec);

    producers.message_type_distribution.resize(producers.count);

    for (const auto &[key, val] : j.at("producers").at("distribution").items())
    {
        uint16_t idx = ParseIndex(key, "msg_type_");

        if (idx >= producers.count)
            throw std::out_of_range("msg type index out of range(producers)");

        producers.message_type_distribution[idx] = val;
    }
}

void FromJson(const json &j, ProcessorsConfig &processors)
{
    j.at("processors").at("count").get_to(processors.count);

    processors.message_type_processing_time_ns.resize(processors.count);

    for (const auto &[key, val] : j.at("processors").at("processing_times_ns").items())
    {
        uint16_t idx = ParseIndex(key, "msg_type_");

        if (idx > processors.count)
            throw std::out_of_range("msg type index out of range(processors)");

        processors.message_type_processing_time_ns[idx] = std::chrono::nanoseconds(val);
    }
}

void FromJson(const json &j, StrategyConfig &strategies)
{
    j.at("strategies").at("count").get_to(strategies.count);

    strategies.strategy_processing_time_ns.resize(strategies.count);

    for (const auto &[key, val] : j.at("strategies").at("processing_times_ns").items())
    {
        uint16_t idx = ParseIndex(key, "strategy_");

        if (idx >= strategies.count)
            throw std::out_of_range("strategy index out of range");

        strategies.strategy_processing_time_ns[idx] = std::chrono::nanoseconds(val);
    }
}

void FromJson(const json &j, std::vector<Stage1Rule> &rules)
{
    rules.reserve(j.at("stage1_rules").size());

    for (const auto &rule : j.at("stage1_rules"))
    {
        rules.emplace_back(
            rule.at("msg_type"),
            rule.at("processors"));
    }
}

void FromJson(const json &j, std::vector<Stage2Rule> &rules)
{
    rules.reserve(j.at("stage2_rules").size());

    for (const auto &rule : j.at("stage2_rules"))
    {
        rules.emplace_back(
            rule.at("msg_type"),
            rule.at("strategy"),
            rule.at("ordering_required"));
    }
}

uint16_t ParseIndex(const std::string &key, const std::string &prefix)
{
    if (!key.starts_with(prefix))
        throw std::runtime_error("invalid key with index");

    return std::stoi(key.substr(prefix.size()));
}