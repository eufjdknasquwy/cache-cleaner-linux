#pragma once
#include "constants.h"
#include <filesystem>
namespace core
{
    class CacheCleaner
    {
        public:
            CacheCleaner() = delete;
            static void sort_cache(const std::vector<std::string> &paths_to_clear);
            static bool exists(const std::string &path_str);

        private:
            static void clear_cache(const std::vector<std::string> &paths_to_clear);
            static void del_to_trash(const std::filesystem::directory_entry &path);
            static void del_to_trash(const std::filesystem::path &path);
            static void run_command(const std::string &command);
    };
} // namespace core
