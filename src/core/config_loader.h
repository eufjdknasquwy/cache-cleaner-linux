#pragma once
#include "danger_level.h"
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <string>
namespace core
{
    struct Config
    {
        public:
            std::map<std::string, DangerLevel> known_paths{};
    };
    class ConfigLoader
    {
        public:
            ConfigLoader() = delete;
            static void load_config();
            static std::vector<std::string> get_paths_by_danger(core::DangerLevel danger);
            static const core::Config &get_config()
            {
                return m_config;
            }

        private:
            static constexpr const char *DEFAULT_USER_CONFIG = R"({
  "categories":
  [
    {
      "category" : "User",
      "danger_level": "User",
      "paths":
      [
      ]
    }
  ]
})";

            static DangerLevel parse_danger_level(const std::string s);
            static core::Config m_config;

            static void load_user_config();
            static void ensure_user_config_exists();
    };
} // namespace core
