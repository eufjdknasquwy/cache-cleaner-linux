#pragma once
#include "core/config_loader.h"
#include "utils/args_parser.h"
#include <gtkmm.h>
namespace ui
{
    class MainWindow
    {
        public:
            MainWindow();
            ~MainWindow() = default;
            void show_window()
            {
                m_window->show_all();
            }
            Gtk::Window &get_window()
            {
                return *m_window;
            }

        private:
            static constexpr int DEFAULT_WIN_WIDTH = 650;
            static constexpr int DEFAULT_WIN_HEIGHT = 800;
            static constexpr int DEFAULT_MARGIN = 10;
            static constexpr int DEFAULT_SPACING = 5;
            static constexpr int DEFAULT_PADDING = 0;

            static constexpr const char *SAFE_EXPANDER_TITLE = "Safe to clear cache";
            static constexpr const char *SAFE_EXPANDER_DESC = "this cache is safe to clear";

            static constexpr const char *WARNING_EXPANDER_TITLE = "This cache requires caution (Warning!)";
            static constexpr const char *WARNING_EXPANDER_DESC = "delete with your own risk";

            static constexpr const char *UNKNOWN_EXPANDER_TITLE = "Unknown cache (Warning!)";
            static constexpr const char *UNKNOWN_EXPANDER_DESC = "delete with your own risk";

            static constexpr const char *SYSTEM_EXPANDER_TITLE = "System cache (root, Warning!)";
            static constexpr const char *SYSTEM_EXPANDER_DESC = "this cache needs root rights";

            static constexpr const char *USER_EXPANDER_TITLE = "User added cache";
            static constexpr const char *USER_EXPANDER_DESC = "your own added cache";

            const core::Config &m_config;
            std::unique_ptr<Gtk::Window> m_window;
            Gtk::Box *m_window_box = nullptr;

            Gtk::Box *m_top_box = nullptr;
            Gtk::ScrolledWindow *m_scroll = nullptr;
            Gtk::Box *m_content_box = nullptr;
            Gtk::Box *m_bottom_box = nullptr;

            Gtk::Expander *m_safe_expander = nullptr;
            Gtk::Expander *m_warning_expander = nullptr;
            Gtk::Expander *m_unknown_expander = nullptr;
            Gtk::Expander *m_system_expander = nullptr;
            Gtk::Expander *m_user_expander = nullptr;
            bool m_expander_expanded = false;

            Gtk::Button *m_close_button = nullptr;
            Gtk::Button *m_scan_cache_button = nullptr;

            void fill_content_box();
            void fill_bottom_box();

            void scan_cache();

            Gtk::Expander *create_category(const std::string &title, const std::string &desc,
                                           const std::vector<std::string> &paths);

            void clear_box(Gtk::Box &box);
            static void set_margin(Gtk::Widget &widget, int size);
    };
} // namespace ui
