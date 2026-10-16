#pragma once
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

        private:
            static PackageManagers s_package_managers;

            static void detect_distribution();
            static void detect_package_managers(const std::string &distro_id);
            static void detect_pm_by_distro_id(const std::string &distro_id);
            static void detect_pm_by_command(const std::string &command, const std::string &pm_name);
    };
} // namespace utils
