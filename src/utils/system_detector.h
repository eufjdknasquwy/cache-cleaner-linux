#pragma once
#include <nlohmann/json.hpp>
namespace utils
{
    struct PackageManagers
    {
            std::set<std::string> names;
    };
    class SystemDetector
    {
        public:
            SystemDetector() = delete;
            static void list_detections();
            static const PackageManagers &get_pms()
            {
                detect_distribution();
                return s_package_managers;
            }
            static void detect_distribution(const nlohmann::json &json_config = {});

        private:
            const nlohmann::json &m_pm_config;
            static PackageManagers s_package_managers;

            static void detect_package_managers(const std::string &distro_id, const nlohmann::json &json_config = {});
            static void detect_pm_by_distro_id(const std::string_view distro_id,
                                               const nlohmann::json &json_config = {});
            static void detect_pm_by_command(const std::string &command, const std::string &pm_name);
    };
} // namespace utils
