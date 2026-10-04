#include "config_loader.h"
#include "constants.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;
namespace fs = std::filesystem;
namespace core
{
    Config ConfigLoader::m_config;
}
void core::ConfigLoader::load_config()
{
    std::ifstream config_file(get_categories_dir().string());
    json json_config = json::parse(config_file, nullptr, true, true);
    auto categories = json_config["categories"];
    m_config.known_paths.clear();
    for (auto cat : categories)
    {
        if (!cat.contains("danger_level") || !cat.contains("paths"))
            continue;
        for (auto path_json : cat["paths"])
        {
            std::string danger_str = cat["danger_level"].get<std::string>();
            DangerLevel danger = parse_danger_level(danger_str);

            std::string path = path_json.get<std::string>();
            m_config.known_paths[path] = danger;
        }
    }
    load_user_config();
}
void core::ConfigLoader::load_user_config()
{
    ensure_user_config_exists();
    std::ifstream config_file(get_user_categories_dir().string());
    json json_config = json::parse(config_file, nullptr, true, true);
    auto categories = json_config["categories"];
    for (auto cat : categories)
    {
        for (auto path_json : cat["paths"])
        {
            std::string path = path_json.get<std::string>();
            m_config.known_paths[path] = DangerLevel::User;
        }
    }
}
void core::ConfigLoader::ensure_user_config_exists()
{
    fs::path config_dir = get_home_dir() / ".config" / "cache-cleaner";
    fs::path config_file = config_dir / "user_categories.json";
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
        out.close();
        std::cout << "Created default config: " << config_file << "\n";
    }
}
core::DangerLevel core::ConfigLoader::parse_danger_level(const std::string d)
{
    if (d == "Safe")
        return DangerLevel::Safe;
    else if (d == "Warning")
        return DangerLevel::Warning;
    else if (d == "Unknown")
        return DangerLevel::Unknown;
    return DangerLevel::Unknown;
}
