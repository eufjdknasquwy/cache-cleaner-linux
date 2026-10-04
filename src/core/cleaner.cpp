#include "core/cleaner.h"
#include "constants.h"
#include <algorithm>
#include <filesystem>
#include <giomm/file.h>
#include <iostream>
namespace fs = std::filesystem;
void core::CacheCleaner::sort_cache(const std::vector<std::string> dirty_paths_to_clear)
{
    std::vector<std::string> filtered_paths;
    for (std::string path : dirty_paths_to_clear)
    {
        if (path[0] == '~') // проверка на home директорию
        {
            std::string clean_path = path;
            clean_path.replace(0, 1, get_home_dir());
            filtered_paths.push_back(clean_path);
        }
    }
    clear_cache(filtered_paths);
}
/** Clears cache
 * @ param paths_to_clear: vector with cache paths, that need to be cleared
 */
void core::CacheCleaner::clear_cache(const std::vector<std::string> paths_to_clear)
{
    std::string yay_dir = get_home_dir() / ".cache/yay";
    std::string paru_dir = get_home_dir() / ".cache/paru";
    for (std::string path : paths_to_clear)
    {
        if (!fs::exists(path))
        {
            std::cout << "File or directory " << path << " does not exist, proceeding" << std::endl;
            continue;
        }
        if (path == yay_dir)
        {
            std::vector<std::string> yay_files_exceptions{"completion.cache", "vcs.json"};
            std::cout << "Deleting yay cache..." << std::endl;
            for (const auto &entry : fs::directory_iterator(yay_dir))
            {
                if (std::ranges::find(yay_files_exceptions, entry.path().filename().string()) ==
                    yay_files_exceptions.end())
                    del_to_trash(entry);
            }
        }
        else if (path == paru_dir)
        {
            std::vector<std::string> paru_files_exceptions{"completion.cache", "vcs.json"};
            std::cout << "Deleting paru cache..." << std::endl;
            for (const auto &entry : fs::directory_iterator(paru_dir))
            {
                if (std::ranges::find(paru_files_exceptions, entry.path().filename().string()) ==
                    paru_files_exceptions.end())
                    del_to_trash(entry);
            }
        }
        else
        {
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
void core::CacheCleaner::del_to_trash(const std::filesystem::directory_entry &entry)
{
    try
    {
        auto file = Gio::File::create_for_path(entry.path());
        file->trash();
        std::cout << "Deleted " << entry.path() << "\n";
    }
    catch (const Glib::Error &e)
    {
        if (e.code() == Gio::Error::PERMISSION_DENIED)
        {
            std::cerr << "Permission denied, skipping " << entry.path() << "\n";
        }
        else
        {
            std::cerr << "Failed deleting " << entry.path() << ": " << e.what() << "\n";
        }
    }
}
