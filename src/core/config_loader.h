#pragma once
#include "core/enums.h"
#include "core/structs.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
using json = nlohmann::json;
namespace core
{
    struct CacheEntry
    {
            std::string name = "";
            std::string desc = "";
            std::string path = "";
            std::string type = "";
            std::string command = "";
            DangerLevel danger = DangerLevel::Unknown;
            bool root = false;
    };
    struct Config
    {
            std::vector<CacheEntry> entries;
    };
    class ConfigLoader
    {
        public:
            ConfigLoader() = delete;
            static void load_config();
            static void load_pm_config();
            static std::map<std::string, std::string> get_paths_by_danger(core::DangerLevel danger);
            static const core::Config &get_config()
            {
                return s_config;
            }
            static void export_report_to_json(std::ofstream &report_file, core::ClearReport &clear_report);

        private:
            static core::Config s_config;

            static constexpr const char *DEFAULT_CATEGORIES_PATH = "/org/example/cache-cleaner/categories.json";
            static constexpr const char *DEFAULT_PACKAGE_MANAGERS_PATH =
                "/org/example/cache-cleaner/package_managers.json";

            static constexpr const char *DEFAULT_USER_CONFIG = R"({
  "categories":
  [
    {
      "entries":
      [
        { "name": "example-cache", "path": "~/.cache/example", "type": "path" }
      ]
    }
  ]
})";

            static void fill_cache_entry(core::CacheEntry &cache, const json &entry, core::DangerLevel danger);
            static DangerLevel parse_danger_level(const std::string_view d);

            static void load_user_config();
            static int load_json_file(json &json_config, const std::string &path);
            static json load_embedded_json(const std::string &resource_path);
            static void ensure_user_config_exists();
    };
} // namespace core
