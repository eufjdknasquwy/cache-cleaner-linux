#include <filesystem>
namespace core
{
    std::filesystem::path get_home_dir();
    std::filesystem::path get_categories_dir();
    std::filesystem::path get_user_categories_dir();
    std::filesystem::path get_package_managers_dir();
    std::filesystem::path get_css_style_path();
} // namespace core
