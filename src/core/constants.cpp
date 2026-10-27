#include <filesystem>
#include <glibmm.h>
namespace fs = std::filesystem;
namespace core
{
    fs::path get_home_dir() // возвращает /home/user
    {
        return Glib::get_home_dir();
    }
    fs::path project_root()
    {
#ifdef PROJECT_ROOT_DIR
        return fs::path(PROJECT_ROOT_DIR);
#else
        auto cwd = fs::current_path();
        if (cwd.filename() == "build" || cwd.filename() == "bin")
        {
            return cwd.parent_path();
        }
        return cwd;
#endif
    }
    fs::path get_categories_dir()
    {
        return project_root() / "src" / "config" / "categories.json";
    }
    fs::path get_user_categories_dir()
    {
        return get_home_dir() / ".config" / "cache-cleaner" / "user_categories.json";
    }
    fs::path get_package_managers_dir()
    {
        return project_root() / "src" / "config" / "package_managers.json";
    }
    fs::path get_css_style_path()
    {
        return project_root() / "src" / "config" / "app_style.css";
    }
    std::string format_size(std::uintmax_t bytes)
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
} // namespace core
