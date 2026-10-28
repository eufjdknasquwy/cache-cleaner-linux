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
            static constexpr int MIN_WIN_WIDTH = 550;
            static constexpr int MIN_WIN_HEIGHT = 500;

            static constexpr int DEFAULT_WIN_WIDTH = 700;
            static constexpr int DEFAULT_WIN_HEIGHT = 650;

            static constexpr int DEFAULT_MARGIN = 10;
            static constexpr int DEFAULT_SPACING = 5;
            static constexpr int DEFAULT_PADDING = 0;
            static constexpr int DEFAULT_INDENT = 40;
            static constexpr int DEFAULT_ICON_SIZE = 24;

            static constexpr bool DEFAULT_DIALOG_MODAL = false;
            static constexpr bool DEFAULT_DIALOG_MARKUP = false;

            static constexpr const char *DEFAULT_CLOSE_BUTTON_ICON = "window-close";
            static constexpr const char *DEFAULT_SCAN_CACHE_BUTTON_ICON = "view-refresh";
            static constexpr const char *DEFAULT_CLEAR_TRASH_BUTTON_ICON = "user-trash";

            static constexpr std::uintmax_t UNKNOWN_SIZE = UINT32_MAX;

            static constexpr const char *SAFE_EXPANDER_TITLE = "Safe cache";
            static constexpr const char *SAFE_EXPANDER_DESC = "This cache is safe to clear";

            static constexpr const char *WARNING_EXPANDER_TITLE = "Unsafe cache";
            static constexpr const char *WARNING_EXPANDER_DESC =
                "This cache requires caution, delete with your own risk (Warning!)";

            static constexpr const char *UNKNOWN_EXPANDER_TITLE = "Unknown cache (Warning!)";
            static constexpr const char *UNKNOWN_EXPANDER_DESC = "Unknown cache, delete with your own risk (Warning!)";

            static constexpr const char *SYSTEM_EXPANDER_TITLE = "System cache";
            static constexpr const char *SYSTEM_EXPANDER_DESC =
                "This cache contains package managers and more (Warning!)\n\nNote, that if you do not have a GUI "
                "polkit agent, you\nshould launch program with sudo or use CLI mode in sudo";

            static constexpr const char *USER_EXPANDER_TITLE = "User cache";
            static constexpr const char *USER_EXPANDER_DESC = "Your own added cache";

            const core::Config &m_config;
            std::unique_ptr<Gtk::Window> m_window;
            Glib::Dispatcher m_dispatcher;
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
            Gtk::Button *m_clear_trash_button = nullptr;

            void fill_content_box();
            void fill_bottom_box();

            void scan_cache();
            void on_scan_cache_finished();
            void scan_size();
            void clear_trash_bin();

            Gtk::Expander *create_category(const std::string &title, const std::string &desc,
                                           const std::map<std::string, std::string> &paths);

            bool confirm_deletion(std::vector<std::string> paths_to_clear, bool is_safe);
            bool create_single_confirmation(const char *message);
            bool create_double_confirmation(const char *message1, const char *message2);

            struct CacheRow
            {
                    bool selected = false;
                    std::string name;
                    std::string desc;
                    std::string path;
                    std::string command;
                    bool root;
                    std::uintmax_t bytes;
                    std::string size_str;
            };

            class ModelColumns : public Gtk::TreeModelColumnRecord
            {
                public:
                    ModelColumns()
                    {
                        add(column_selected);
                        add(column_name);
                        add(column_desc);
                        add(column_path);
                        add(column_command);
                        add(column_root);
                        add(column_size);
                    }
                    Gtk::TreeModelColumn<bool> column_selected;
                    Gtk::TreeModelColumn<std::string> column_name;
                    Gtk::TreeModelColumn<std::string> column_desc;
                    Gtk::TreeModelColumn<std::string> column_path;
                    Gtk::TreeModelColumn<std::string> column_command;
                    Gtk::TreeModelColumn<bool> column_root;
                    Gtk::TreeModelColumn<std::string> column_size;
            };
    };
} // namespace ui
