#include "constants.h"
#include <filesystem>
namespace core
{
    class CacheCleaner
    {
        public:
            CacheCleaner() = delete;
            static void sort_cache(const std::vector<std::string> &paths_to_clear);
            static bool exists(const std::string &path_str)
            {
                if (!path_str.empty() && path_str[0] == '~')
                {
                    auto path_str2 = path_str;
                    path_str2.replace(0, 1, get_home_dir().string());
                    std::filesystem::path path = std::filesystem::path(path_str2).lexically_normal();
                    return std::filesystem::exists(path);
                }
                return std::filesystem::exists(std::filesystem::path(path_str).lexically_normal());
            }

        private:
            static void clear_cache(const std::vector<std::string> &paths_to_clear);
            static void del_to_trash(const std::filesystem::directory_entry &path);
            static void del_to_trash(const std::filesystem::path &path);
    };
} // namespace core
