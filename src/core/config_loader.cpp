#include "config_loader.h"
#include "constants.h"
#include "core/cleaner.h"
#include "core/enums.h"
#include "utils/console_logger.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
using json = nlohmann::json;
namespace fs = std::filesystem;
namespace core
{
    Config ConfigLoader::s_config;
} // namespace core
/** Loads config from json
 */
std::map<std::string, std::string> core::ConfigLoader::get_paths_by_danger(core::DangerLevel danger)
{
    std::map<std::string, std::string> names_and_paths;
    for (const auto &entry : s_config.entries)
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
            throw std::runtime_error("Failed openning config");
        json_config = json::parse(config_file, nullptr, true, true);
        return 0;
    }
    catch (const std::runtime_error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to open settings file: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        return 1;
    }
    catch (const json::parse_error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to parse settings file: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Parse error at byte " + std::to_string(e.byte)));
        return 1;
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error,
                                  "Unexpected error occured while trying to parse settings file: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
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

    s_config.entries.clear();
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

            std::string name = entry["name"].get<std::string>();
            if (name != "")
                cache.name = name;
            else
                cache.name = entry["path"].get<std::string>();

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

            s_config.entries.push_back(cache);
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

            std::string name = entry["name"].get<std::string>();
            if (name != "")
                cache.name = name;
            else
                cache.name = entry["path"].get<std::string>();

            std::string path = entry["path"].get<std::string>();
            if (path != "")
                cache.path = path;
            else
                cache.path = cache.name;

            if (entry.contains("type"))
                cache.type = entry["type"].get<std::string>();

            if (cache.type == "command" && entry.contains("command"))
                cache.command = entry["command"].get<std::string>();

            cache.danger = DangerLevel::User;

            s_config.entries.push_back(cache);
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
            utils::ConsoleLogger::log(core::LogLevel::Error,
                                      ("Cannot create " + config_dir.string() + ": " + ec.message()));
            return;
        }
        utils::ConsoleLogger::log(core::LogLevel::Info, ("Created directory: " + config_dir.string()));
    }
    if (!fs::exists(config_file))
    {
        std::ofstream out(config_file);
        if (!out.is_open())
        {
            utils::ConsoleLogger::log(core::LogLevel::Error, ("Cannot create " + config_file.string()));
            return;
        }
        out << DEFAULT_USER_CONFIG;
        utils::ConsoleLogger::log(core::LogLevel::Info, ("Created default config: " + config_file.string()));
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
    if (d == "User")
        return DangerLevel::User;

    return DangerLevel::Unknown;
}
void core::ConfigLoader::export_report_to_json(std::ofstream &report_file, core::ClearReport &clear_report)
{
    json j;

    for (int i = 0; i < clear_report.paths.size(); ++i)
    {
        j["paths"][std::string("path" + std::to_string(i + 1))] = clear_report.paths[i];
    }
    for (const auto &[command, code] : clear_report.commands)
    {
        j["commands"][command] = code;
    }

    report_file << j.dump(4);
}
