#include "system_detector.h"
#include "utils/console_logger.h"
#include <fstream>
#include <string>
using json = nlohmann::json;
utils::PackageManagers utils::SystemDetector::s_package_managers;
void utils::SystemDetector::list_detections()
{
    auto package_managers = get_pms();
    for (std::string_view pm : package_managers.names)
    {
        utils::ConsoleLogger::log(core::LogLevel::Info, ("Detected root cache: " + std::string(pm)));
    }
}
void utils::SystemDetector::detect_distribution(const json &json_config)
{
    std::ifstream f("/etc/os-release");
    std::string line;
    std::string id;
    while (std::getline(f, line))
    {
        if (line.starts_with("ID="))
        {
            id = line.substr(3);
            if (!id.empty() && id.front() == '"')
                id = id.substr(1, id.size() - 2);
        }
    }
    detect_package_managers(id, json_config);
}
void utils::SystemDetector::detect_package_managers(const std::string &distro_id, const json &json_config)
{
    // system package managers
    detect_pm_by_distro_id(distro_id, json_config);

    // other package managers
    detect_pm_by_command("which flatpak", "flatpak");
    detect_pm_by_command("which snap", "snap");
    detect_pm_by_command("which journalctl", "journalctl");
    detect_pm_by_command("which docker", "docker");
    detect_pm_by_command("which podman", "podman");
}
void utils::SystemDetector::detect_pm_by_distro_id(const std::string_view distro_id, const json &json_config)
{
    if (json_config.is_null())
        return;

    const auto package_managers = json_config["package_managers"];
    for (const auto &[pm_name, distros] : package_managers.items())
    {
        for (const auto &distro : distros)
        {
            if (distro.is_string() && distro.get<std::string>() == distro_id)
            {
                s_package_managers.names.insert(pm_name);
                break;
            }
        }
    }
}
void utils::SystemDetector::detect_pm_by_command(const std::string &command, const std::string &pm_name)
{
    try
    {
        int ret = 0;
        std::string out, err;
        Glib::spawn_command_line_sync(command, &out, &err, &ret);
        if (ret == 0)
        {
            s_package_managers.names.insert(pm_name);
        }
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error,
                                  ("Unexpected error occured while trying to determine existence of package manager '" +
                                   pm_name + "' by command '" + command + "'"));
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Message: " + std::string(e.what())));
        utils::ConsoleLogger::flush_errors_to_ui();
    }
}
