#include "core/cleaner.h"
#include "core/config_loader.h"
#include "core/constants.h"
#include "core/enums.h"
#include "utils/console_logger.h"
#include "utils/system_detector.h"
#include <cstdint>
#include <filesystem>
#include <future>
#include <string>
#include <thread>
#include <unordered_set>
namespace fs = std::filesystem;
core::Config const &core::CacheCleaner::s_config = core::ConfigLoader::get_config();
core::ClearReport core::CacheCleaner::s_del_result;
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
    const auto &entries = core::ConfigLoader::get_config().entries;
    auto it = std::find_if(entries.begin(), entries.end(), [&](const auto &e) { return e.path == path_str; });

    if (it == entries.end())
        return false;

    if (it->danger == core::DangerLevel::System)
    {
        auto pms = utils::SystemDetector::get_pms();
        return pms.names.contains(path_str);
    }

    if (path_str.empty())
        return true;

    std::string resolved = path_str;
    if (resolved[0] == '~')
        resolved.replace(0, 1, get_home_dir().string());

    return fs::exists(fs::path(resolved).lexically_normal());
}
uintmax_t core::CacheCleaner::size(const std::string &path_str)
{
    auto path = expand_tilde(path_str);
    std::error_code ec;
    for (const auto &entry : s_config.entries)
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
/** Sorts out cache in std::vector and then calls the clear_cache function
 * @ param dirty_paths_to_clear: std::vector with unparsed cache paths
 */
core::ClearReport core::CacheCleaner::sort_cache(const std::vector<std::string> &dirty_paths_to_clear,
                                                 const ClearOptions &options)
{
    std::vector<std::string> filtered_paths;
    const std::string home_path_str = get_home_dir().string();
    for (const std::string &path : dirty_paths_to_clear)
    {
        if (!path.empty())
        {
            filtered_paths.push_back(expand_tilde(path));
        }
    }
    clear_cache(filtered_paths, options);
    return s_del_result;
}
/** Clears cache
 * @ param paths_to_clear: vector with cache paths, that need to be cleared
 */
void core::CacheCleaner::clear_cache(const std::vector<std::string> &paths_to_clear, const ClearOptions &options)
{
    if (options.cli)
        Gio::init();
    s_del_result.paths.clear();
    s_del_result.commands.clear();
    const fs::path home_path = get_home_dir();

    // aur helpers
    fs::path yay_dir = (home_path / ".cache/yay").lexically_normal();
    fs::path paru_dir = (home_path / ".cache/paru").lexically_normal();

    std::unordered_set<std::string> root_paths;
    std::map<std::string, std::string> command_paths;

    for (const auto &entry : s_config.entries)
    {
        std::string parsed_path = expand_tilde(entry.path);
        if (entry.root)
        {
            root_paths.insert(parsed_path);
        }
        if (entry.type == "command" || entry.command != "")
        {
            command_paths[parsed_path] = entry.command;
        }
    }
    static const std::unordered_set<std::string> AUR_EXCEPTIONS{"completion.cache", "vcs.json"};
    for (const std::string_view path_str : paths_to_clear)
    {
        fs::path path = fs::path(path_str).lexically_normal();
        if (!command_paths.contains(path) && !root_paths.contains(path) && !fs::exists(path))
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
                    delete_paths(entry, options);
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
                    delete_paths(entry, options);
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
                delete_paths(std::string(path), options);
            }
            else
            {
                delete_paths(fs::directory_entry(path), options);
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

    struct CommandResult
    {
            int exit_code = 1;
            std::string stdout_str = "";
            std::string stderr_str = "";
    };
    CommandResult result;

    try
    {
        std::future<void> run_command_async = std::async(
            std::launch::async, [command, &result]() -> void
            { Glib::spawn_command_line_sync(command, &result.stdout_str, &result.stderr_str, &result.exit_code); });

        run_command_async.get();

        if (result.exit_code == 0)
            utils::ConsoleLogger::log(core::LogLevel::Info, ("Command succeeded: " + command));
        else
            utils::ConsoleLogger::log(
                core::LogLevel::Error,
                ("Command failed (status " + std::to_string(result.exit_code) + "): " + result.stderr_str));
    }
    catch (const Glib::Error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Failed to run " + command + ": " + std::string(e.what())));
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Code: " + std::to_string(e.code())));
        result.exit_code = e.code();
        utils::ConsoleLogger::flush_errors_to_ui();
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, ("Failed to run " + command + ": " + e.what()));
        utils::ConsoleLogger::flush_errors_to_ui();
    }
    s_del_result.commands[command] = result.exit_code;
}
void core::CacheCleaner::delete_paths(const fs::directory_entry &entry, const ClearOptions &options)
{
    try
    {
        std::string path = entry.path().string();
        del_to_trash(path, options);
        // std::thread thread(del_to_trash, path, options);
        // thread.join();
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to delete file: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, "Message: " + std::string(e.what()));
        utils::ConsoleLogger::flush_errors_to_ui();
    }
}
void core::CacheCleaner::delete_paths(const fs::path &fs_path, const ClearOptions &options)
{
    try
    {
        std::string path = fs_path.string();
        del_to_trash(path, options);
        // std::thread thread(del_to_trash, path, options);
        // thread.join();
    }
    catch (const std::exception &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to delete file: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, "Message: " + std::string(e.what()));
        utils::ConsoleLogger::flush_errors_to_ui();
    }
}
void core::CacheCleaner::del_to_trash(const std::string &path, const core::ClearOptions &options)
{
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
        s_del_result.paths.push_back(path);
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            utils::ConsoleLogger::log(core::LogLevel::Warning,
                                      ("Permission denied, skipping " + path + ", try running with sudo"));
            utils::ConsoleLogger::flush_errors_to_ui();
        }
        else
        {
            utils::ConsoleLogger::log(core::LogLevel::Error,
                                      ("Failed to delete " + path + ": " + std::string(e.what())));
            utils::ConsoleLogger::flush_errors_to_ui();
        }
    }
}
void core::CacheCleaner::clear_trash_bin()
{
    std::thread(
        []
        {
            std::string trash_path = Glib::get_user_data_dir() + "/Trash";
            try
            {
                for (const auto &entry : fs::directory_iterator(trash_path + "/files"))
                    fs::remove_all(entry.path());

                for (const auto &entry : fs::directory_iterator(trash_path + "/info"))
                    fs::remove_all(entry.path());
            }
            catch (const fs::filesystem_error &e)
            {
                utils::ConsoleLogger::log(core::LogLevel::Error, "Error cleaning user trash bin: ");
                utils::ConsoleLogger::log(core::LogLevel::Error, "Message: " + std::string(e.what()));
                utils::ConsoleLogger::log(core::LogLevel::Error, "Code: " + std::to_string(e.code().value()) + " (" +
                                                                     e.code().message() + ")");
                utils::ConsoleLogger::flush_errors_to_ui();
            }
            catch (const std::exception &e)
            {
                utils::ConsoleLogger::log(core::LogLevel::Error,
                                          "Unexpected error occured while trying to clear user trash bin: ");
                utils::ConsoleLogger::log(core::LogLevel::Error, std::string("Message: ") + e.what());
                utils::ConsoleLogger::flush_errors_to_ui();
            }
        })
        .detach();
}
