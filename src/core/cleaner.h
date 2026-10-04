#include <filesystem>
namespace core
{
    class CacheCleaner
    {
        public:
            CacheCleaner() = delete;
            static void sort_cache(const std::vector<std::string> paths_to_clear);

        private:
            static void clear_cache(const std::vector<std::string> paths_to_clear);
            static void del_to_trash(const std::filesystem::directory_entry &path);
    };
} // namespace core
