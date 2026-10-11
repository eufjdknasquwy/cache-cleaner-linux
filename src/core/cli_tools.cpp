#include "cli_tools.h"
#include "core/cleaner.h"
#include "core/config_loader.h"
#include <iostream>
bool core::cli::CliTools::ask_confirmation(const char *prompt)
{
    std::cout << prompt << " [yes/no]: ";
    std::string answer;
    std::getline(std::cin, answer);
    return answer == "yes" || answer == "y";
}
void core::cli::CliTools::delete_category(core::DangerLevel danger)
{
    core::ConfigLoader::load_config();
    auto dirty_paths = core::ConfigLoader::get_paths_by_danger(danger);
    std::vector<std::string> paths;

    for (const auto &[name, path] : dirty_paths)
    {
        if (core::CacheCleaner::exists(path))
            paths.push_back(path);
    }
    if (paths.size() <= 0)
    {
        std::cout << "No cache to clear" << "\n";
        return;
    }
    if (danger == core::DangerLevel::Safe)
    {
        std::cout << "About to clear " << paths.size() << " paths (Safe):\n";
        for (const auto &p : paths)
            std::cout << "  " << p << "\n";

        if (!ask_confirmation("Continue?"))
            return;

        core::CacheCleaner::sort_cache(paths);
        return;
    }

    std::cout << "WARNING: You are about to clear " << paths.size() << " paths:\n";
    for (const auto &p : paths)
        std::cout << "  " << p << "\n";
    std::cout << "This may break some programs.\n\n";

    if (!ask_confirmation("Are you sure?"))
        return;

    if (!ask_confirmation("Are you REALLY sure? This is not reversible"))
        return;

    core::CacheCleaner::sort_cache(paths);
}
void core::cli::CliTools::list_category(core::DangerLevel danger)
{
    core::ConfigLoader::load_config();
    auto paths = core::ConfigLoader::get_paths_by_danger(danger);
}
std::vector<std::string> core::cli::CliTools::list_all_categories()
{
    core::ConfigLoader::load_config();
    auto safe_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Safe);
    auto warning_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Warning);
    auto unknown_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Unknown);
    auto system_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::System);
    auto user_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::User);
    std::vector<std::string> all_paths;
    for (const auto &[name, path] : safe_paths)
    {
        if (core::CacheCleaner::exists(path))
            all_paths.push_back(path);
    }
    for (const auto &[name, path] : warning_paths)
    {
        if (core::CacheCleaner::exists(path))
            all_paths.push_back(path);
    }
    for (const auto &[name, path] : unknown_paths)
    {
        if (core::CacheCleaner::exists(path))
            all_paths.push_back(path);
    }
    for (const auto &[name, path] : system_paths)
    {
        if (core::CacheCleaner::exists(path))
            all_paths.push_back(path);
    }
    for (const auto &[name, path] : user_paths)
    {
        if (core::CacheCleaner::exists(path))
            all_paths.push_back(path);
    }
    return all_paths;
}
