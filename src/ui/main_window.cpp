#include "ui/main_window.h"
#include "core/cleaner.h"
#include "core/config_loader.h"
#include "core/enums.h"
#include "ui/css.h"
#include "ui/message_dialog_helper.h"
#include "ui_helper.h"
#include "utils/console_logger.h"
#include <cstdint>
#include <future>
#include <gtkmm/revealer.h>
#include <string>
ui::MainWindow::MainWindow() : m_config(core::ConfigLoader::get_config())
{
    m_window = std::make_unique<Gtk::Window>();
    m_window->set_title("Cache cleaner");
    m_window->set_size_request(MIN_WIN_WIDTH, MIN_WIN_HEIGHT);
    m_window->set_default_size(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

    m_window->signal_delete_event().connect(
        [](GdkEventAny *event) -> bool
        {
            auto app = Gtk::Application::get_default();
            if (app)
                app->quit();
            return true;
        }); // closes program

    Gtk::Label *title = Gtk::manage(new Gtk::Label()); // window title
    title->set_markup("<b>Cache cleaner</b>");
    title->set_halign(Gtk::ALIGN_START);

    m_dispatcher.connect(sigc::mem_fun(*this, &ui::MainWindow::on_scan_cache_finished));

    m_window_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, DEFAULT_SPACING)); // контейнер на весь экран
    m_window->add(*m_window_box);
    ui::UIHelper::set_margin(*m_window_box, DEFAULT_MARGIN);

    // разделение окна на 3 контейнера
    m_top_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, DEFAULT_SPACING));
    m_scroll = Gtk::manage(new Gtk::ScrolledWindow());
    m_scroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_content_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, DEFAULT_SPACING));
    m_bottom_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, DEFAULT_SPACING));

    // добавление 3 контейнеров окна
    m_window_box->pack_start(*m_top_box, false, false, DEFAULT_PADDING);
    m_scroll->add(*m_content_box);
    m_window_box->pack_start(*m_scroll, true, true, DEFAULT_PADDING);
    m_window_box->pack_end(*m_bottom_box, false, false, DEFAULT_PADDING);

    m_top_box->pack_start(*title, false, false, DEFAULT_PADDING);

    fill_content_box(); // fills middle part of the window
    fill_bottom_box();  // fills bottom menu of the window
    Css::apply_css();
}
void ui::MainWindow::fill_content_box()
{
    auto safe_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Safe);
    auto warning_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Warning);
    auto unknown_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Unknown);
    auto system_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::System);
    auto user_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::User);
    if (m_safe_expander || m_warning_expander || m_unknown_expander || m_user_expander)
    {
        m_safe_expander = nullptr;
        m_warning_expander = nullptr;
        m_unknown_expander = nullptr;
        m_system_expander = nullptr;
        m_user_expander = nullptr;
    }

    if (!safe_paths.empty())
        m_safe_expander = create_category(SAFE_EXPANDER_TITLE, SAFE_EXPANDER_DESC, safe_paths);
    if (!warning_paths.empty())
        m_warning_expander = create_category(WARNING_EXPANDER_TITLE, WARNING_EXPANDER_DESC, warning_paths);
    if (!unknown_paths.empty())
        m_unknown_expander = create_category(UNKNOWN_EXPANDER_TITLE, UNKNOWN_EXPANDER_DESC, unknown_paths);
    if (!system_paths.empty())
        m_system_expander = create_category(SYSTEM_EXPANDER_TITLE, SYSTEM_EXPANDER_DESC, system_paths);
    if (!user_paths.empty())
        m_user_expander = create_category(USER_EXPANDER_TITLE, USER_EXPANDER_DESC, user_paths);

    if (m_safe_expander)
        m_content_box->pack_start(*m_safe_expander, false, false, 5);
    if (m_warning_expander)
        m_content_box->pack_start(*m_warning_expander, false, false, 5);
    if (m_unknown_expander)
        m_content_box->pack_start(*m_unknown_expander, false, false, 5);
    if (m_system_expander)
        m_content_box->pack_start(*m_system_expander, false, false, 5);
    if (m_user_expander)
        m_content_box->pack_start(*m_user_expander, false, false, 5);
}
void ui::MainWindow::fill_bottom_box()
{
    m_close_button = Gtk::manage(new Gtk::Button("Close"));
    m_close_button->signal_clicked().connect([this]() { m_window->close(); });
    ui::UIHelper::set_button_icon(*m_close_button, DEFAULT_CLOSE_BUTTON_ICON, DEFAULT_ICON_SIZE);
    m_bottom_box->pack_end(*m_close_button, false, false, 5);

    m_scan_cache_button = Gtk::manage(new Gtk::Button("Scan cache"));
    m_scan_cache_button->signal_clicked().connect(
        [this]()
        {
            if (!m_scan_cache_button->get_sensitive())
                return;

            m_scan_cache_button->set_sensitive(false);
            m_scan_cache_button->set_label("Scanning cache...");

            std::thread(
                [this]()
                {
                    try
                    {
                        scan_cache();
                    }
                    catch (const std::exception &e)
                    {
                        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to scan cache: ");
                        utils::ConsoleLogger::log(core::LogLevel::Error, ("Message: " + std::string(e.what())));
                        utils::ConsoleLogger::flush_errors_to_ui();
                    }
                    m_dispatcher.emit();
                })
                .detach();
        });
    ui::UIHelper::set_button_icon(*m_scan_cache_button, DEFAULT_SCAN_CACHE_BUTTON_ICON, DEFAULT_ICON_SIZE);
    m_bottom_box->pack_end(*m_scan_cache_button, false, false, 5);

    m_clear_trash_button = Gtk::manage(new Gtk::Button("Clear trash"));
    m_clear_trash_button->signal_clicked().connect([this]() { clear_trash_bin(); });
    ui::UIHelper::set_button_icon(*m_clear_trash_button, DEFAULT_CLEAR_TRASH_BUTTON_ICON, DEFAULT_ICON_SIZE);
    m_bottom_box->pack_end(*m_clear_trash_button, false, false, 5);
}
void ui::MainWindow::scan_cache()
{
    core::ConfigLoader::load_config(); // scans caches
}
void ui::MainWindow::on_scan_cache_finished() // update all categories
{
    ui::UIHelper::clear_box(*m_content_box); // clear categories
    fill_content_box();                      // fill categories

    m_safe_expander = nullptr;
    m_warning_expander = nullptr;
    m_unknown_expander = nullptr;
    m_system_expander = nullptr;
    m_user_expander = nullptr;

    m_content_box->show_all();
    m_scan_cache_button->set_label("Scan cache");
    m_scan_cache_button->set_sensitive(true);
}
void ui::MainWindow::clear_trash_bin()
{
    create_single_confirmation("Are you sure you want to clear trash bin?");
    std::thread thread(core::CacheCleaner::clear_trash_bin);
    thread.join();
}
Gtk::Expander *ui::MainWindow::create_category(const std::string &title, const std::string &desc,
                                               const std::map<std::string, std::string> &paths)
{
    Gtk::Expander *expander = Gtk::manage(new Gtk::Expander(title));

    Gtk::ScrolledWindow *scroll = Gtk::manage(new Gtk::ScrolledWindow());
    scroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scroll->set_max_content_height(320);
    scroll->set_propagate_natural_height(true);

    Gtk::Box *listbox = Gtk::manage(new Gtk::Box());

    Gtk::ListBoxRow *listbox_row = Gtk::manage(new Gtk::ListBoxRow());

    Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, DEFAULT_SPACING));
    ui::UIHelper::set_margin(*box, DEFAULT_MARGIN);

    Gtk::Label *desc_label = Gtk::manage(new Gtk::Label(desc));
    desc_label->set_halign(Gtk::ALIGN_START);
    box->add(*desc_label);

    // treeview creation

    auto columns = std::make_shared<ModelColumns>();
    Glib::RefPtr<Gtk::ListStore> ref_list_store = Gtk::ListStore::create(*columns);

    Gtk::TreeView *treeview = Gtk::manage(new Gtk::TreeView());
    treeview->set_model(ref_list_store);

    auto *toggle = Gtk::manage(new Gtk::CellRendererToggle());

    toggle->signal_toggled().connect(
        [this, columns, ref_list_store, toggle](const Glib::ustring &path)
        {
            auto iter = ref_list_store->get_iter(Gtk::TreeModel::Path(path));
            if (iter)
            {
                auto row = *iter;
                row[columns->column_selected] = !row[columns->column_selected];
            }
        });

    int toggle_column = treeview->append_column("", *toggle);
    treeview->get_column(toggle_column - 1)->add_attribute(toggle->property_active(), columns->column_selected);

    treeview->append_column("Name", columns->column_name);
    treeview->append_column("Description", columns->column_desc);
    treeview->append_column("Path", columns->column_path);
    treeview->append_column("Command", columns->column_command);
    treeview->append_column("Root", columns->column_root);
    treeview->append_column("Size", columns->column_size);

    box->add(*treeview);

    //

    Gtk::Box *bottom_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, DEFAULT_SPACING));

    expander->add(*scroll);
    scroll->add(*listbox);
    listbox->add(*listbox_row);
    listbox_row->add(*box);

    std::vector<CacheRow> cache_rows;
    std::unordered_map<std::string, std::future<std::uintmax_t>> futures;
    for (const auto &[name, path] : paths)
    {
        if (!core::CacheCleaner::exists(path))
        {
            if (std::string(PROJECT_BUILD_TYPE) == "Debug")
                utils::ConsoleLogger::log(core::LogLevel::Warning,
                                          ("name: " + name + ", path: " + path + " doesn't exist"));
            continue;
        }

        if (std::string(PROJECT_BUILD_TYPE) == "Debug")
            utils::ConsoleLogger::log(core::LogLevel::Info, ("name: " + name + ", path: " + path + " exists"));

        futures[name] = (std::async(std::launch::async, core::CacheCleaner::size, path));
    }
    for (const auto &[name, path] : paths)
    {
        auto it = futures.find(name);
        if (it == futures.end())
            continue;

        std::uintmax_t bytes = UNKNOWN_SIZE;
        try
        {
            bytes = it->second.get();
        }
        catch (const std::exception &e)
        {
            utils::ConsoleLogger::log(core::LogLevel::Error, ("Failed to get size of cache " + name + " : "));
            utils::ConsoleLogger::log(core::LogLevel::Error, ("Message: " + std::string(e.what())));
            utils::ConsoleLogger::flush_errors_to_ui();
        }

        std::string size_str = bytes == UNKNOWN_SIZE ? "unknown" : "(" + core::CacheCleaner::format_size(bytes) + ")";

        std::string cache_desc;
        std::string command;
        bool root;

        for (const auto &cache_entry : m_config.entries)
        {
            if (name == cache_entry.name && path == cache_entry.path)
            {
                cache_desc = cache_entry.desc;
                command = cache_entry.command;
                root = cache_entry.root;
            }
        }

        cache_rows.push_back({false, name, cache_desc, path, command, root, bytes, size_str});
    }
    std::sort(cache_rows.begin(), cache_rows.end(),
              [](const CacheRow &a, const CacheRow &b)
              {
                  bool a_unknown = (a.bytes == UNKNOWN_SIZE);
                  bool b_unknown = (b.bytes == UNKNOWN_SIZE);

                  if (a_unknown != b_unknown)
                      return !a_unknown;

                  return a.bytes > b.bytes;
              });
    for (const auto &cache_row : cache_rows) // adds cache rows
    {
        const auto row = *(ref_list_store->append());
        row[columns->column_selected] = cache_row.selected;
        row[columns->column_name] = cache_row.name;
        row[columns->column_desc] = cache_row.desc;
        row[columns->column_path] = cache_row.name == cache_row.path ? "" : cache_row.path;
        row[columns->column_command] = cache_row.command;
        row[columns->column_root] = cache_row.root;
        row[columns->column_size] = cache_row.size_str;
    }

    box->add(*bottom_box);
    Gtk::Button *clear_selected = Gtk::manage(new Gtk::Button("Clear selected"));
    Gtk::CheckButton *choose_everything = Gtk::manage(new Gtk::CheckButton("Choose everything"));

    bottom_box->pack_start(*clear_selected, false, false, 5);
    bottom_box->pack_start(*choose_everything, false, false, 5);

    cache_rows.size() > 0 ? expander->show_all() : expander->hide();

    clear_selected->signal_clicked().connect(
        [this, box, expander, columns, ref_list_store, paths]()
        {
            std::vector<std::string> paths_to_clear;
            auto children = ref_list_store->children();
            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                auto row = *iter;
                if (row[columns->column_selected])
                {
                    std::string name = row[columns->column_name];
                    std::string path = row[columns->column_path];
                    std::string final_path = path.empty() ? name : path;
                    paths_to_clear.push_back(final_path);
                }
            }
            bool deletion_confirmed = confirm_deletion(paths_to_clear, expander->get_label() == SAFE_EXPANDER_TITLE);
            if (deletion_confirmed)
            {
                auto result = core::CacheCleaner::sort_cache(paths_to_clear);
                if (!result.paths.empty() || !result.commands.empty())
                    ui::MessageDialogHelper::spawn_result_dialog(result);
            }

            for (auto iter = children.begin(); iter != children.end();
                 ++iter) // sets false to all toggles that were on after cleaning
            {
                auto row = *iter;
                if (row[columns->column_selected])
                    row[columns->column_selected] = false;
            }
        });

    choose_everything->signal_toggled().connect(
        [box, choose_everything, columns, ref_list_store]()
        {
            auto children = ref_list_store->children();
            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                auto row = *iter;
                row[columns->column_selected] = choose_everything->get_active();
            }
        });

    return expander;
}
bool ui::MainWindow::confirm_deletion(std::vector<std::string> paths_to_clear, bool is_safe)
{
    if (is_safe) // one confirmation for safe category before deleting
    {
        if (paths_to_clear.size() <= 0)
            return false;

        return create_single_confirmation("Are you sure you want to clear this cache?");
    }
    else // double confirmation for other categories before deleting
    {
        if (paths_to_clear.size() <= 0)
            return false;

        bool dialog_response =
            create_double_confirmation("Are you sure you want to clear this cache?\nThis may break some programs",
                                       "Totally sure?\nThis can break your system");
        if (!dialog_response)
            return false;

        return dialog_response;
    }
}
bool ui::MainWindow::create_single_confirmation(const char *message)
{
    Gtk::MessageDialog dialog(*m_window, message, DEFAULT_DIALOG_MARKUP, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO,
                              DEFAULT_DIALOG_MODAL);

    return dialog.run() == Gtk::RESPONSE_YES;
}
bool ui::MainWindow::create_double_confirmation(const char *message1, const char *message2)
{
    Gtk::MessageDialog dialog1(*m_window, message1, DEFAULT_DIALOG_MARKUP, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO,
                               DEFAULT_DIALOG_MODAL);

    if (dialog1.run() != Gtk::RESPONSE_YES)
        return false;

    Gtk::MessageDialog dialog2(*m_window, message2, DEFAULT_DIALOG_MARKUP, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO,
                               DEFAULT_DIALOG_MODAL);

    return dialog2.run() == Gtk::RESPONSE_YES;
}
