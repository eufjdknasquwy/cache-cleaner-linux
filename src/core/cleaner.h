#pragma once
#include "constants.h"
#include "core/config_loader.h"
#include "structs.h"
#include <filesystem>
namespace core
{
    class CacheCleaner
    {
        public:
            CacheCleaner() = delete;

            struct ClearReport
            {
                    std::set<std::string> paths;
                    std::map<std::string, int> commands;
            };

            static ClearReport sort_cache(const std::vector<std::string> &paths_to_clear,
                                          const core::ClearOptions &options = {});
            static bool exists(const std::string &path_str);
            static uintmax_t size(const std::string &path_str);
            static std::string format_size(std::uintmax_t bytes);

        private:
            static ClearReport m_del_result;
            static const Config &m_config;

            static std::string expand_tilde(const std::string &path);
            static void clear_cache(const std::vector<std::string> &paths_to_clear, const core::ClearOptions &options);
            static void del_to_trash(const std::filesystem::directory_entry &path, const core::ClearOptions &options);
            static void del_to_trash(const std::filesystem::path &path, const core::ClearOptions &options);
            static void run_command(const std::string &command, const core::ClearOptions &options);
    };
} // namespace core
