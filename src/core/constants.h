#include <filesystem>
namespace fs = std::filesystem;
namespace core
{
    fs::path get_home_dir();
    fs::path get_categories_dir();
    fs::path get_user_categories_dir();
    fs::path get_package_managers_dir();
    fs::path get_css_style_path();
    std::string format_size(std::uintmax_t bytes);
} // namespace core
