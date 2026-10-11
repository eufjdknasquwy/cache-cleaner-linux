#include "main_window.h"
#include "core/cleaner.h"
#include "core/config_loader.h"
#include "message_dialog_helper.h"
#include "ui_helper.h"
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
        }); // подписка на сигнал закрытия окна (закрывает программу)

    Gtk::Label *title = Gtk::manage(new Gtk::Label()); // рамка
    title->set_markup("<b>Cache cleaner</b>");
    title->set_halign(Gtk::ALIGN_START);

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

    fill_content_box(); // заполнение середины окна
    fill_bottom_box();  // заполнение нижней части окна
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
    m_bottom_box->pack_end(*m_close_button, false, false, 5);

    m_scan_cache_button = Gtk::manage(new Gtk::Button("Scan cache"));
    m_scan_cache_button->signal_clicked().connect([this]() { scan_cache(); });
    m_bottom_box->pack_end(*m_scan_cache_button, false, false, 5);

    m_scan_size_button = Gtk::manage(new Gtk::Button("Scan size"));
    m_scan_size_button->signal_clicked().connect([this]() { scan_size(); });
    m_bottom_box->pack_end(*m_scan_size_button, false, false, 5);
}
void ui::MainWindow::scan_cache() // обновить все категории и ихние кэши
{
    core::ConfigLoader::load_config();       // пересканирование путей из файла
    ui::UIHelper::clear_box(*m_content_box); // очистка категорий
    fill_content_box();                      // заполнение категорий

    m_safe_expander = nullptr;
    m_warning_expander = nullptr;
    m_unknown_expander = nullptr;
    m_system_expander = nullptr;
    m_user_expander = nullptr;

    m_content_box->show_all();
}
void ui::MainWindow::scan_size()
{
}
Gtk::Expander *ui::MainWindow::create_category(const std::string &title, const std::string &desc,
                                               const std::map<std::string, std::string> &paths)
{
    Gtk::Expander *expander = Gtk::manage(new Gtk::Expander(title));
    expander->set_expanded(false);

    Gtk::ScrolledWindow *scroll = Gtk::manage(new Gtk::ScrolledWindow());
    scroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scroll->set_max_content_height(320);
    scroll->set_propagate_natural_height(true);

    Gtk::Box *listbox = Gtk::manage(new Gtk::Box());
    // listbox->set_selection_mode(Gtk::SELECTION_NONE);

    Gtk::ListBoxRow *listbox_row = Gtk::manage(new Gtk::ListBoxRow());

    Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, DEFAULT_SPACING));
    ui::UIHelper::set_margin(*box, DEFAULT_MARGIN);
    Gtk::Box *bottom_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, DEFAULT_SPACING));

    expander->add(*scroll);
    scroll->add(*listbox);
    listbox->add(*listbox_row);
    listbox_row->add(*box);

    Gtk::Label *desc_label = Gtk::manage(new Gtk::Label(desc));
    desc_label->set_halign(Gtk::ALIGN_START);
    box->add(*desc_label);

    std::vector<std::string> existing_paths;
    for (const auto &[name, path] : paths)
    {
        if (!core::CacheCleaner::exists(path))
            continue;
        existing_paths.push_back(path);

        std::uintmax_t bytes = core::CacheCleaner::size(path);
        std::string size_str = bytes == UINT32_MAX ? "unknown" : "(" + core::CacheCleaner::format_size(bytes) + ")";
        auto check_button_label = name + "\t" + "  " + size_str;

        Gtk::CheckButton *check_button = Gtk::manage(new Gtk::CheckButton(check_button_label));
        box->add(*check_button);
    }

    box->add(*bottom_box);
    Gtk::Button *clear_selected = Gtk::manage(new Gtk::Button("Clear selected"));
    Gtk::CheckButton *choose_everything = Gtk::manage(new Gtk::CheckButton("Choose everything"));

    bottom_box->pack_start(*clear_selected, false, false, 5);
    bottom_box->pack_start(*choose_everything, false, false, 5);

    if (existing_paths.size() > 0)
    {
        expander->show_all();
    }
    else
    {
        expander->hide();
    }

    clear_selected->signal_clicked().connect(
        [this, box, expander, paths]()
        {
            std::vector<std::string> paths_to_clear;
            for (Gtk::Widget *widget : box->get_children())
            {
                if (Gtk::CheckButton *check_button = dynamic_cast<Gtk::CheckButton *>(widget))
                {
                    if (check_button->get_active())
                    {
                        auto cache_name = check_button->get_label();
                        paths_to_clear.push_back(paths.at(cache_name));
                    }
                }
                else
                    continue;
            }
            bool is_safe = (expander->get_label() == SAFE_EXPANDER_TITLE);
            if (is_safe)
            {
                if (paths_to_clear.size() <= 0)
                    return;
                Gtk::MessageDialog dialog("Are you sure you want to clear this cache?", false, Gtk::MESSAGE_QUESTION,
                                          Gtk::BUTTONS_YES_NO, false);

                if (dialog.run() == Gtk::RESPONSE_YES)
                {
                    auto result = core::CacheCleaner::sort_cache(paths_to_clear);
                    if (!result.paths.empty() || !result.commands.empty())
                        ui::MessageDialogHelper::spawn_result_dialog(result);
                }
            }
            else
            {
                if (paths_to_clear.size() <= 0)
                    return;
                Gtk::MessageDialog dialog1("Are you sure you want to clear this cache?\nThis may break some programs.",
                                           false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO, false);

                if (dialog1.run() != Gtk::RESPONSE_YES)
                    return;

                Gtk::MessageDialog dialog2("Totally sure?\nThis can break your system.", false, Gtk::MESSAGE_WARNING,
                                           Gtk::BUTTONS_YES_NO, false);

                if (dialog2.run() == Gtk::RESPONSE_YES)
                {
                    auto result = core::CacheCleaner::sort_cache(paths_to_clear);
                    if (!result.paths.empty() || !result.commands.empty())
                        ui::MessageDialogHelper::spawn_result_dialog(result);
                }
            }

            for (Gtk::Widget *widget : box->get_children())
            {
                if (Gtk::CheckButton *check_button = dynamic_cast<Gtk::CheckButton *>(widget))
                {
                    if (check_button->get_active())
                        check_button->set_active(false);
                }
                else
                    continue;
            }
        });

    choose_everything->signal_toggled().connect(
        [box, choose_everything]()
        {
            for (Gtk::Widget *widget : box->get_children())
            {
                if (Gtk::CheckButton *check_button = dynamic_cast<Gtk::CheckButton *>(widget))
                {
                    check_button->set_active(choose_everything->get_active());
                }
                else
                    continue;
            }
        });

    return expander;
}
