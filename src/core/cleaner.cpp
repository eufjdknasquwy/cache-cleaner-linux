#include "core/cleaner.h"
#include "config_loader.h"
#include "constants.h"
#include "utils/system_detector.h"
#include <filesystem>
#include <giomm/file.h>
#include <iostream>
namespace fs = std::filesystem;
bool core::CacheCleaner::exists(const std::string &path_str)
{
    auto paths = core::ConfigLoader::get_config().known_paths;
    auto it = paths.find(path_str);
    if (it == paths.end())
        return false;
    auto path_danger = it->second;
    if (path_danger != core::DangerLevel::System)
    {
        if (!path_str.empty() && path_str[0] == '~')
        {
            std::string path_str2 = path_str;
            path_str2.replace(0, 1, get_home_dir().string());
            fs::path path = fs::path(path_str2).lexically_normal();
            return fs::exists(path);
        }
        return fs::exists(fs::path(path_str).lexically_normal());
    }
    else
    {
        auto package_managers = utils::SystemDetector::get_pms();
        if (package_managers.names.size() > 0)
        {
            if (package_managers.names.contains(path_str))
                return true;
            else
                return false;
        }
        return false;
    }
}
/** Sorts out cache in std::vector and then calls the clear_cache function
 * @ param dirty_paths_to_clear: std::vector with unparsed cache paths
 */
void core::CacheCleaner::sort_cache(const std::vector<std::string> &dirty_paths_to_clear)
{
    std::vector<std::string> filtered_paths;
    const std::string home_path_str = get_home_dir().string();
    for (std::string path : dirty_paths_to_clear)
    {
        if (!path.empty() && path[0] == '~') // проверка на home директорию
        {
            std::string clean_path = path;
            clean_path.replace(0, 1, home_path_str);
            filtered_paths.push_back(clean_path);
        }
        else
        {
            filtered_paths.push_back(path);
        }
    }
    clear_cache(filtered_paths);
}
/** Clears cache
 * @ param paths_to_clear: vector with cache paths, that need to be cleared
 */
void core::CacheCleaner::clear_cache(const std::vector<std::string> &paths_to_clear)
{
    const fs::path home_path = get_home_dir();

    // aur helpers
    fs::path yay_dir = (home_path / ".cache/yay").lexically_normal();
    fs::path paru_dir = (home_path / ".cache/paru").lexically_normal();

    // package managers and programming languages
    fs::path pip_dir = (home_path / ".cache/pip").lexically_normal();
    fs::path go_dir = (home_path / ".cache/go-build").lexically_normal();
    fs::path nuget_dir = (home_path / ".nuget/packages").lexically_normal();
    fs::path npm_dir = (home_path / ".npm/_cacache").lexically_normal();

    fs::path pacman = "pacman";
    fs::path apt = "apt";
    fs::path dnf = "dnf";

    fs::path flatpak = "flatpak";
    fs::path snap = "snap";

    fs::path journalctl = "journalctl (systemd logs)";

    static const std::set<std::string> root_paths = {pacman.string(),  apt.string(),  dnf.string(),
                                                     flatpak.string(), snap.string(), journalctl.string()};
    static const std::map<std::string, std::string> command_paths = {
        {pip_dir.string(), "pip cache purge"},
        {go_dir.string(), "go clean -cache -fuzzcache"},
        {nuget_dir.string(), "dotnet nuget locals all --clear"},
        {npm_dir.string(), "npm cache clean --force"},
        {pacman.string(), "pacman -Scc --noconfirm"},
        {apt.string(), "sh -c 'apt clean && apt autoclean'"},
        {dnf.string(), "dnf clean all"},
        {flatpak.string(), "flatpak uninstall --unused"},
        {snap.string(), "rm -rf /var/lib/snapd/cache/*"},
        {journalctl.string(), "journalctl --vacuum-time=2weeks"},
    };
    static const std::set<std::string> AUR_EXCEPTIONS{"completion.cache", "vcs.json"};

    for (const std::string &path_str : paths_to_clear)
    {
        fs::path path = fs::path(path_str).lexically_normal();
        if (!root_paths.contains(path) && !fs::exists(path))
        {
            std::cout << "File or directory " << path << " does not exist, proceeding" << std::endl;
            continue;
        }
        if (fs::exists(yay_dir) && fs::exists(path) && fs::equivalent(path, yay_dir))
        {
            std::cout << "Deleting yay cache..." << std::endl;
            for (const auto &entry : fs::directory_iterator(yay_dir))
            {
                if (!AUR_EXCEPTIONS.contains(entry.path().filename().string()))
                    del_to_trash(entry);
            }
        }
        else if (fs::exists(paru_dir) && fs::exists(path) && fs::equivalent(path, paru_dir))
        {
            std::cout << "Deleting paru cache..." << std::endl;
            for (const auto &entry : fs::directory_iterator(paru_dir))
            {
                if (!AUR_EXCEPTIONS.contains(entry.path().filename().string()))
                    del_to_trash(entry);
            }
        }
        else
        {
            auto it = command_paths.find(path);
            if (it != command_paths.end())
            {
                std::string command = "";
                if (root_paths.contains(path))
                {
                    command = "pkexec " + it->second;
                }
                else
                {
                    command = it->second;
                }
                run_command(command);
                continue;
            }
            std::cout << "Deleting " << path << "\n";
            if (fs::is_directory(path))
            {
                for (const auto &entry : fs::directory_iterator(path))
                    del_to_trash(entry);
            }
            else
            {
                del_to_trash(fs::directory_entry(path));
            }
        }
    }
}
void core::CacheCleaner::run_command(const std::string &command)
{
    std::cout << "Running: " << command << "\n";
    try
    {
        int ret = 0;
        std::string out, err;
        Glib::spawn_command_line_sync(command, &out, &err, &ret);
        if (ret != 0)
            std::cerr << "Command failed (" << ret << "): " << err << "\n";
    }
    catch (const Glib::Error &e)
    {
        std::cerr << "Failed to run " << command << ": " << e.what() << "\n";
    }
}
void core::CacheCleaner::del_to_trash(const fs::directory_entry &entry)
{
    try
    {
        auto file = Gio::File::create_for_path(entry.path().string());
        file->trash();
        std::cout << "Deleted " << entry.path() << "\n";
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            std::cerr << "Permission denied, skipping " << entry.path() << ", try running with sudo" << "\n";
        }
        else
        {
            std::cerr << "Failed deleting " << entry.path() << ": " << e.what() << "\n";
        }
    }
}
void core::CacheCleaner::del_to_trash(const fs::path &path)
{
    try
    {
        auto file = Gio::File::create_for_path(path.string());
        file->trash();
        std::cout << "Deleted " << path << "\n";
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            std::cerr << "Permission denied, skipping " << path << ", try running with sudo" << "\n";
        }
        else
        {
            std::cerr << "Failed deleting " << path << ": " << e.what() << "\n";
        }
    }
}
