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
            static constexpr int DEFAULT_WIN_WIDTH = 800;
            static constexpr int DEFAULT_WIN_HEIGHT = 600;
            static constexpr int DEFAULT_MARGIN = 10;
            static constexpr int DEFAULT_SPACING = 5;
            static constexpr int DEFAULT_PADDING = 0;

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
            Gtk::Expander *m_user_expander = nullptr;

            Gtk::Button *m_close_button = nullptr;
            Gtk::Button *m_scan_cache_button = nullptr;

            std::vector<std::string> get_paths_by_danger(core::DangerLevel danger);
            void fill_content_box();
            void fill_bottom_box();

            void scan_cache();

            Gtk::Expander *create_category(const std::string &title, const std::string &desc,
                                           const std::vector<std::string> &paths);

            void clear_box(Gtk::Box &box);
            static void set_margin(Gtk::Widget &widget, int size);
    };
} // namespace ui
