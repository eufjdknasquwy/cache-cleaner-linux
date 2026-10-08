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
            static const PackageManagers &get_pms()
            {
                detect_distribution();
                return m_package_managers;
            }

        private:
            static PackageManagers m_package_managers;

            static void detect_distribution();
            static void detect_package_manager(const std::string &distro_id);
            static void detect_pm_by_command(const std::string &command, const std::string &pm_name);
    };
} // namespace utils
