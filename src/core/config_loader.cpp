#include "config_loader.h"
#include "constants.h"
#include "core/cleaner.h"
#include "core/enums.h"
#include "utils/console_logger.h"
#include "utils/system_detector.h"
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
int core::ConfigLoader::load_json_file(json &json_config, const std::string &path)
{
    try
    {
        std::ifstream config_file;
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
void core::ConfigLoader::fill_cache_entry(core::CacheEntry &cache, const json &entry, core::DangerLevel danger)
{
    try
    {
        std::string name = entry.value("name", "");
        cache.name = name.empty() ? entry.value("path", "") : name;

        cache.desc = entry.value("desc", "");

        std::string path = entry.value("path", "");
        cache.path = path.empty() ? name : path;

        if (entry.contains("type"))
            cache.type = entry.value("type", "path");

        if (cache.type == "command" || entry.contains("command"))
            cache.command = entry.value("command", "");

        cache.danger = danger;

        cache.root = entry.value("root", false);
    }
    catch (json::type_error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error,
                                  "JSON type error occured while trying to create clear report: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
        return;
    }
    catch (const json::exception &e)
    {
        utils::ConsoleLogger::log(LogLevel::Error, "JSON error occured while trying to create clear report: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
        return;
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(
            LogLevel::Error, std::string("Unexpected error occured while trying to create clear report: ") + e.what());
        return;
    }
}
void core::ConfigLoader::load_config()
{
    json json_config;
    if (load_json_file(json_config, get_categories_dir().string()) != 0)
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
            const std::string danger_str = category.value("danger_level", "Unknown");
            const DangerLevel danger = parse_danger_level(danger_str);

            CacheEntry cache;
            fill_cache_entry(cache, entry, danger);
            s_config.entries.push_back(cache);
        }
    }
    load_user_config(); // loads user config from user json
}
void core::ConfigLoader::load_user_config()
{
    ensure_user_config_exists(); // makes sure user config exists

    json json_config;
    if (load_json_file(json_config, get_user_categories_dir().string()) != 0)
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
            fill_cache_entry(cache, entry, DangerLevel::User);
            s_config.entries.push_back(cache);
        }
    }
}
void core::ConfigLoader::load_pm_config()
{
    json json_config;
    if (load_json_file(json_config, get_package_managers_dir().string()) != 0)
        return;

    utils::SystemDetector::detect_distribution(json_config);
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
    try
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
        try
        {
            report_file << j.dump(4);
            if (report_file.fail())
            {
                utils::ConsoleLogger::log(LogLevel::Error, "Failed writing to clear report file");
            }
        }
        catch (const json::type_error &e)
        {
            utils::ConsoleLogger::log(LogLevel::Error, "JSON dump error occured while trying to create clear report: ");
            utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
            utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
            return;
        }
    }
    catch (json::type_error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error,
                                  "JSON type error occured while trying to create clear report: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
        return;
    }
    catch (const json::exception &e)
    {
        utils::ConsoleLogger::log(LogLevel::Error, "JSON error occured while trying to create clear report: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
        utils::ConsoleLogger::log(core::LogLevel::Error, ("ID: " + std::to_string(e.id)));
        return;
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(
            LogLevel::Error, std::string("Unexpected error occured while trying to create clear report: ") + e.what());
        return;
    }
}
