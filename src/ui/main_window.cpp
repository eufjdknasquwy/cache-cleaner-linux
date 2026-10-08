#include "main_window.h"
#include "core/cleaner.h"
#include "core/config_loader.h"
#include <string>
ui::MainWindow::MainWindow() : m_config(core::ConfigLoader::get_config())
{
    m_window = std::make_unique<Gtk::Window>();
    m_window->set_title("Cache cleaner");
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
    set_margin(*m_window_box, DEFAULT_MARGIN);

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
    std::vector<std::string> safe_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Safe);
    std::vector<std::string> warning_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Warning);
    std::vector<std::string> unknown_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::Unknown);
    std::vector<std::string> system_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::System);
    std::vector<std::string> user_paths = core::ConfigLoader::get_paths_by_danger(core::DangerLevel::User);
    if (m_safe_expander || m_warning_expander || m_unknown_expander || m_user_expander)
    {
        m_safe_expander = nullptr;
        m_warning_expander = nullptr;
        m_unknown_expander = nullptr;
        m_system_expander = nullptr;
        m_user_expander = nullptr;
    }
    m_safe_expander = create_category(SAFE_EXPANDER_TITLE, SAFE_EXPANDER_DESC, safe_paths);
    m_warning_expander = create_category(WARNING_EXPANDER_TITLE, WARNING_EXPANDER_DESC, warning_paths);
    m_unknown_expander = create_category(UNKNOWN_EXPANDER_TITLE, UNKNOWN_EXPANDER_DESC, unknown_paths);
    m_system_expander = create_category(SYSTEM_EXPANDER_TITLE, SYSTEM_EXPANDER_DESC, system_paths);
    m_user_expander = create_category(USER_EXPANDER_TITLE, USER_EXPANDER_DESC, user_paths);

    m_content_box->pack_start(*m_safe_expander, false, false, 5);
    m_content_box->pack_start(*m_warning_expander, false, false, 5);
    m_content_box->pack_start(*m_unknown_expander, false, false, 5);
    m_content_box->pack_start(*m_system_expander, false, false, 5);
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
}
void ui::MainWindow::scan_cache() // обновить все категории и ихние кэши
{
    core::ConfigLoader::load_config(); // пересканирование путей из файла
    clear_box(*m_content_box);         // очистка категорий
    fill_content_box();                // заполнение категорий
    m_content_box->show_all();
}
Gtk::Expander *ui::MainWindow::create_category(const std::string &title, const std::string &desc,
                                               const std::vector<std::string> &paths)
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
    set_margin(*box, DEFAULT_MARGIN);
    Gtk::Box *bottom_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, DEFAULT_SPACING));

    expander->add(*scroll);
    scroll->add(*listbox);
    listbox->add(*listbox_row);
    listbox_row->add(*box);

    Gtk::Label *desc_label = Gtk::manage(new Gtk::Label(desc));
    desc_label->set_halign(Gtk::ALIGN_START);
    box->add(*desc_label);

    for (const std::string &path : paths)
    {
        if (!core::CacheCleaner::exists(path))
            continue;
        Gtk::CheckButton *check_button = Gtk::manage(new Gtk::CheckButton(path));
        box->add(*check_button);
    }

    box->add(*bottom_box);
    Gtk::Button *clear_selected = Gtk::manage(new Gtk::Button("Clear selected"));
    Gtk::CheckButton *choose_everything = Gtk::manage(new Gtk::CheckButton("Choose everything"));

    bottom_box->pack_start(*clear_selected, false, false, 5);
    bottom_box->pack_start(*choose_everything, false, false, 5);

    clear_selected->signal_clicked().connect(
        [this, box, expander]()
        {
            std::vector<std::string> paths_to_clear;
            for (Gtk::Widget *widget : box->get_children())
            {
                if (Gtk::CheckButton *check_button = dynamic_cast<Gtk::CheckButton *>(widget))
                {
                    if (check_button->get_active())
                        paths_to_clear.push_back(check_button->get_label());
                }
                else
                    continue;
            }
            bool is_safe = (expander->get_label() == SAFE_EXPANDER_TITLE);
            if (is_safe)
            {
                Gtk::MessageDialog dialog("Are you sure you want to clear this cache?", false, Gtk::MESSAGE_QUESTION,
                                          Gtk::BUTTONS_YES_NO, false);

                if (dialog.run() == Gtk::RESPONSE_YES)
                    core::CacheCleaner::sort_cache(paths_to_clear);
            }
            else
            {
                Gtk::MessageDialog dialog1("Are you sure you want to clear this cache?\nThis may break some programs.",
                                           false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO, false);

                if (dialog1.run() != Gtk::RESPONSE_YES)
                    return;

                Gtk::MessageDialog dialog2("Totally sure?\nThis can break your system.", false, Gtk::MESSAGE_WARNING,
                                           Gtk::BUTTONS_YES_NO, false);

                if (dialog2.run() == Gtk::RESPONSE_YES)
                    core::CacheCleaner::sort_cache(paths_to_clear);
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
void ui::MainWindow::clear_box(Gtk::Box &box)
{
    auto children = box.get_children();
    for (auto *child : children)
    {
        box.remove(*child);
    }
}
void ui::MainWindow::set_margin(Gtk::Widget &widget, int size)
{
    widget.set_margin_top(size);
    widget.set_margin_bottom(size);
    widget.set_margin_start(size);
    widget.set_margin_end(size);
}
