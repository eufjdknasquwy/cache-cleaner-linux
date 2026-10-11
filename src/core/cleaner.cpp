#include "core/cleaner.h"
#include "config_loader.h"
#include "constants.h"
#include "core/enums.h"
#include "utils/console_logger.h"
#include "utils/system_detector.h"
#include <cstdint>
#include <filesystem>
#include <giomm/file.h>
#include <string>
namespace fs = std::filesystem;
core::Config const &core::CacheCleaner::m_config = core::ConfigLoader::get_config();
core::CacheCleaner::ClearReport core::CacheCleaner::m_del_result;
std::string core::CacheCleaner::expand_tilde(const std::string &path)
{
    if (!path.empty() && path[0] == '~')
    {
        std::string clean_path = path;
        clean_path.replace(0, 1, get_home_dir().string());
        return clean_path;
    }
    return path;
}
bool core::CacheCleaner::exists(const std::string &path_str)
{
    std::map<std::string, DangerLevel> paths;
    for (const auto &cache : core::ConfigLoader::get_config().entries)
    {
        paths[cache.path] = cache.danger;
    }
    auto it = paths.find(path_str);
    if (it == paths.end())
        return false;
    const DangerLevel path_danger = it->second;
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
uintmax_t core::CacheCleaner::size(const std::string &path_str)
{
    auto path = expand_tilde(path_str);
    std::error_code ec;
    for (const auto &entry : m_config.entries)
    {
        if (entry.danger == DangerLevel::System && (entry.path == path_str || entry.path == path))
        {
            return UINT32_MAX;
        }
    }
    if (!fs::is_directory(path))
    {
        auto size = fs::file_size(path, ec);
        if (ec)
        {
            utils::ConsoleLogger::log(core::LogLevel::Error, ec.message());
        }
        return size;
    }
    std::uintmax_t total = 0;
    for (const auto &entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, ec))
    {
        if (ec)
        {
            ec.clear();
            continue;
        }
        if (entry.is_regular_file(ec))
        {
            auto s = entry.file_size(ec);
            if (!ec)
                total += s;
        }
    }
    return total;
}
std::string core::CacheCleaner::format_size(std::uintmax_t bytes)
{
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = static_cast<double>(bytes);
    int unit = 0;

    while (size >= 1024.0 && unit < 4)
    {
        size /= 1024.0;
        ++unit;
    }

    char buf[32];
    if (unit == 0)
        std::snprintf(buf, sizeof(buf), "%ju %s", bytes, units[unit]);
    else if (unit == 1)
        std::snprintf(buf, sizeof(buf), "%.0f %s", size, units[unit]);
    else
        std::snprintf(buf, sizeof(buf), "%.1f %s", size, units[unit]);

    return buf;
}
/** Sorts out cache in std::vector and then calls the clear_cache function
 * @ param dirty_paths_to_clear: std::vector with unparsed cache paths
 */
core::CacheCleaner::ClearReport core::CacheCleaner::sort_cache(const std::vector<std::string> &dirty_paths_to_clear,
                                                               const ClearOptions &options)
{
    std::vector<std::string> filtered_paths;
    const std::string home_path_str = get_home_dir().string();
    for (const std::string &path : dirty_paths_to_clear)
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
    clear_cache(filtered_paths, options);
    return m_del_result;
}
/** Clears cache
 * @ param paths_to_clear: vector with cache paths, that need to be cleared
 */
void core::CacheCleaner::clear_cache(const std::vector<std::string> &paths_to_clear, const ClearOptions &options)
{
    if (options.cli)
        Gio::init();
    m_del_result.paths.clear();
    m_del_result.commands.clear();
    const fs::path home_path = get_home_dir();

    // aur helpers
    fs::path yay_dir = (home_path / ".cache/yay").lexically_normal();
    fs::path paru_dir = (home_path / ".cache/paru").lexically_normal();

    std::set<std::string> root_paths;
    std::map<std::string, std::string> command_paths;

    for (const auto &entry : m_config.entries)
    {
        if (entry.danger == DangerLevel::System)
            root_paths.insert(entry.name);

        if (entry.type == "command" && entry.command != "")
            command_paths[entry.name] = entry.command;
    }
    static const std::set<std::string> AUR_EXCEPTIONS{"completion.cache", "vcs.json"};
    for (const std::string &path_str : paths_to_clear)
    {
        fs::path path = fs::path(path_str).lexically_normal();
        if (!root_paths.contains(path) && !fs::exists(path))
        {
            if (!options.dry_run)
            {
                utils::ConsoleLogger::log(core::LogLevel::Warning,
                                          "File or directory " + path.string() + " does not exist, proceeding");
            }
            continue;
        }
        if (path == yay_dir)
        {
            if (!options.dry_run)
                utils::ConsoleLogger::log(core::LogLevel::Info, "Deleting yay cache...");
            else
                utils::ConsoleLogger::printmsg("[dry-run] Would delete yay cache");

            for (const auto &entry : fs::directory_iterator(yay_dir))
            {
                if (!AUR_EXCEPTIONS.contains(entry.path().filename().string()))
                    del_to_trash(entry, options);
            }
        }
        else if (path == paru_dir)
        {
            if (!options.dry_run)
                utils::ConsoleLogger::log(core::LogLevel::Info, "Deleting paru cache...");
            else
                utils::ConsoleLogger::printmsg("[dry-run] Would delete paru cache");

            for (const auto &entry : fs::directory_iterator(paru_dir))
            {
                if (!AUR_EXCEPTIONS.contains(entry.path().filename().string()))
                    del_to_trash(entry, options);
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
                run_command(command, options);
                continue;
            }
            if (!options.dry_run)
                utils::ConsoleLogger::log(core::LogLevel::Info, ("Deleting " + path.string()));
            if (fs::is_directory(path))
            {
                del_to_trash(path, options);
            }
            else
            {
                del_to_trash(fs::directory_entry(path), options);
            }
        }
    }
}
void core::CacheCleaner::run_command(const std::string &command, const ClearOptions &options)
{
    if (options.dry_run)
    {
        utils::ConsoleLogger::printmsg(("[dry-run] Would run: " + command));
        return;
    }
    utils::ConsoleLogger::log(core::LogLevel::Info, ("Running: " + command));
    int ret = 0;
    std::string out, err;
    try
    {
        Glib::spawn_command_line_sync(command, &out, &err, &ret);
        if (ret == 0)
            utils::ConsoleLogger::log(core::LogLevel::Info, ("Command succeeded: " + command));
        else
            utils::ConsoleLogger::log(core::LogLevel::Error,
                                      ("Command failed (status " + std::to_string(ret) + "): " + err));
    }
    catch (const Glib::Error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Failed to run " + command + ": " + e.what()));
    }
    m_del_result.commands[command] = ret;
}
void core::CacheCleaner::del_to_trash(const fs::directory_entry &entry, const ClearOptions &options)
{
    auto path = entry.path().string();
    if (options.dry_run)
    {
        utils::ConsoleLogger::printmsg(("[dry-run] Would delete: " + path));
        return;
    }
    try
    {
        auto file = Gio::File::create_for_path(path);
        file->trash();
        utils::ConsoleLogger::log(core::LogLevel::Info, ("Deleted " + path));
        m_del_result.paths.insert(path);
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            utils::ConsoleLogger::log(core::LogLevel::Warning, ("Permission denied, skipping " + entry.path().string() +
                                                                ", try running with sudo"));
        }
        else
        {
            utils::ConsoleLogger::log(core::LogLevel::Error,
                                      ("Failed deleting " + entry.path().string() + ": " + e.what()));
        }
    }
}
void core::CacheCleaner::del_to_trash(const fs::path &fs_path, const ClearOptions &options)
{
    auto path = fs_path.string();
    if (options.dry_run)
    {
        utils::ConsoleLogger::printmsg(("[dry-run] Would delete: " + path));
        return;
    }
    try
    {
        auto file = Gio::File::create_for_path(path);
        file->trash();
        utils::ConsoleLogger::log(core::LogLevel::Info, ("Deleted " + path));
        m_del_result.paths.insert(path);
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            utils::ConsoleLogger::log(core::LogLevel::Warning,
                                      ("Permission denied, skipping " + path + ", try running with sudo"));
        }
        else
        {
            utils::ConsoleLogger::log(core::LogLevel::Error, ("Failed deleting " + path + ": " + e.what()));
        }
    }
}
