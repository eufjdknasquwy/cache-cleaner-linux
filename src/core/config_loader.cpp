#include "config_loader.h"
#include "constants.h"
#include "core/cleaner.h"
#include "core/danger_level.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
using json = nlohmann::json;
namespace fs = std::filesystem;
namespace core
{
    Config ConfigLoader::m_config;
} // namespace core
/** Loads config from json
 */
std::map<std::string, std::string> core::ConfigLoader::get_paths_by_danger(core::DangerLevel danger)
{
    std::map<std::string, std::string> names_and_paths;
    for (const auto &entry : m_config.entries)
    {
        if (entry.danger == danger)
            names_and_paths[entry.name] = entry.path;
    }
    return names_and_paths;
}
int core::ConfigLoader::load_json_file(std::ifstream &config_file, json &json_config, const std::string &path)
{
    try
    {
        config_file.open(path);
        if (!config_file.is_open())
            throw std::runtime_error("Cannot open config");
        json_config = json::parse(config_file, nullptr, true, true);
        return 0;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Failed to open settings file: " << "\n";
        std::cerr << "Message: " << e.what() << "\n";
        return 1;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Failed to parse settings file: " << "\n";
        std::cerr << "Message: " << e.what() << "\n";
        std::cerr << "ID: " << e.id << "\n";
        std::cerr << "Parse error at byte " << e.byte << "\n";
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Caught an unexpected error: " << "\n";
        std::cerr << "Message: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
void core::ConfigLoader::load_config()
{
    std::ifstream config_file;
    json json_config;
    if (load_json_file(config_file, json_config, get_categories_dir().string()) != 0)
        return;

    m_config.entries.clear();
    const auto categories = json_config["categories"];
    for (const auto &category : categories)
    {
        if (!category.contains("danger_level") || !category.contains("entries"))
            continue;
        const auto entries = category["entries"];
        for (const auto &entry : entries)
        {
            const std::string danger_str = category["danger_level"].get<std::string>();
            const DangerLevel danger = parse_danger_level(danger_str);

            CacheEntry cache;

            cache.name = entry["name"].get<std::string>();

            std::string path = entry["path"].get<std::string>();
            if (path != "")
                cache.path = path;
            else
                cache.path = cache.name;

            if (entry.contains("type"))
                cache.type = entry["type"].get<std::string>();

            if (cache.type == "command" && entry.contains("command"))
                cache.command = entry["command"].get<std::string>();

            cache.danger = danger;

            m_config.entries.push_back(cache);
        }
    }
    load_user_config(); // loads user config from user json
}
void core::ConfigLoader::load_user_config()
{
    ensure_user_config_exists(); // makes sure user config exists

    std::ifstream config_file;
    json json_config;
    if (load_json_file(config_file, json_config, get_user_categories_dir().string()) != 0)
        return;

    const auto categories = json_config["categories"];
    for (const auto &category : categories)
    {
        if (!category.contains("entries"))
            continue;
        const auto entries = category["entries"];
        for (const auto &entry : entries)
        {
            CacheEntry cache;

            cache.name = entry["name"].get<std::string>();
            cache.path = entry["path"].get<std::string>();
            cache.type = entry["type"].get<std::string>();
            if (cache.type == "command" && entry.contains("command"))
                cache.command = entry["command"].get<std::string>();
            cache.danger = DangerLevel::User;

            m_config.entries.push_back(cache);
        }
    }
}
/** Makes sure user config exists
 */
void core::ConfigLoader::ensure_user_config_exists()
{
    const fs::path config_dir = get_home_dir() / ".config" / "cache-cleaner";
    const fs::path config_file = config_dir / "user_categories.json";
    std::error_code ec;

    if (!fs::exists(config_dir))
    {
        fs::create_directories(config_dir, ec);
        if (ec)
        {
            std::cerr << "Cannot create " << config_dir << ": " << ec.message() << "\n";
            return;
        }
        std::cout << "Created directory: " << config_dir << "\n";
    }
    if (!fs::exists(config_file))
    {
        std::ofstream out(config_file);
        if (!out.is_open())
        {
            std::cerr << "Cannot create " << config_file << "\n";
            return;
        }
        out << DEFAULT_USER_CONFIG;
        // out.close();
        std::cout << "Created default config: " << config_file << "\n";
    }
}
/** Converts string danger level to enum type,
 * example: "Safe" => DangerLevel::Safe
 */
core::DangerLevel core::ConfigLoader::parse_danger_level(const std::string &d)
{
    if (d == "Safe")
        return DangerLevel::Safe;
    if (d == "Warning")
        return DangerLevel::Warning;
    if (d == "Unknown")
        return DangerLevel::Unknown;
    if (d == "System")
        return DangerLevel::System;

    return DangerLevel::Unknown;
}
