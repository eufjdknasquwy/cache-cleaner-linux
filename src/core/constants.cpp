#include <filesystem>
#include <glibmm.h>
namespace core
{
    std::filesystem::path get_home_dir() // возвращает /home/user
    {
        return Glib::get_home_dir();
    }
    std::filesystem::path project_root()
    {
#ifdef PROJECT_ROOT_DIR
        return std::filesystem::path(PROJECT_ROOT_DIR);
#else
        return std::filesystem::current_path();
#endif
    }
    std::filesystem::path get_categories_dir()
    {
        return project_root() / "src" / "config" / "categories.json";
    }
    std::filesystem::path get_user_categories_dir()
    {
        return get_home_dir() / ".config" / "cache-cleaner" / "user_categories.json";
    }
} // namespace core
