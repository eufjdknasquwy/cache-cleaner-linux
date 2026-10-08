#include "system_detector.h"
#include <fstream>
#include <string>
utils::PackageManagers utils::SystemDetector::m_package_managers;
void utils::SystemDetector::detect_distribution()
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
    detect_package_manager(id);
}
void utils::SystemDetector::detect_package_manager(const std::string &distro_id)
{
    // system package managers
    if (distro_id == "arch")
        m_package_managers.names.insert("pacman");

    else if (distro_id == "debian")
        m_package_managers.names.insert("apt");

    else if (distro_id == "ubuntu")
        m_package_managers.names.insert("apt");

    else if (distro_id == "fedora")
        m_package_managers.names.insert("dnf");

    // other package managers
    detect_pm_by_command("which flatpak", "flatpak");
    detect_pm_by_command("which snap", "snap");
    detect_pm_by_command("which journalctl", "journalctl (systemd logs)");
}
void utils::SystemDetector::detect_pm_by_command(const std::string &command, const std::string &pm_name)
{
    int ret = 0;
    std::string out, err;
    Glib::spawn_command_line_sync(command, &out, &err, &ret);
    if (ret == 0)
        m_package_managers.names.insert(pm_name);
}
