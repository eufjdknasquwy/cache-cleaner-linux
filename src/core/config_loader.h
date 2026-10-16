#pragma once
#include "core/structs.h"
#include "enums.h"
#include <nlohmann/json_fwd.hpp>
#include <string>
using json = nlohmann::json;
namespace core
{
    struct CacheEntry
    {
            std::string name;
            std::string path;
            std::string type;
            std::string command;
            DangerLevel danger;
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
            static std::map<std::string, std::string> get_paths_by_danger(core::DangerLevel danger);
            static const core::Config &get_config()
            {
                return s_config;
            }
            static void export_report_to_json(std::ofstream &report_file, core::ClearReport &clear_report);

        private:
            static constexpr const char *DEFAULT_USER_CONFIG = R"({
  "categories":
  [
    {
      "category" : "User",
      "danger_level": "User",
      "entries":
      [
      ]
    }
  ]
})";

            static DangerLevel parse_danger_level(const std::string &d);
            static core::Config s_config;

            static void load_user_config();
            static int load_json_file(std::ifstream &config_file, json &json_config, const std::string &path);
            static void ensure_user_config_exists();
    };
} // namespace core
