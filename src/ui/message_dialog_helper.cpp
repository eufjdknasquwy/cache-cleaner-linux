#include "message_dialog_helper.h"
#include "ui_helper.h"
#include "utils/console_logger.h"
#include <atomic>
#include <cstddef>
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
Glib::Dispatcher ui::MessageDialogHelper::s_error_dispatcher;
std::string ui::MessageDialogHelper::s_pending_error;
std::mutex ui::MessageDialogHelper::s_error_mutex;
std::atomic<bool> ui::MessageDialogHelper::s_is_dialog_open{false};
void ui::MessageDialogHelper::init_error_handler()
{
    s_error_dispatcher.connect(
        []()
        {
            if (s_is_dialog_open.exchange(true))
            {
                return;
            }
            std::string msg;
            {
                std::lock_guard<std::mutex> lock(s_error_mutex);
                msg = s_pending_error;
                s_pending_error.clear();
            }
            spawn_error_dialog(msg);
            s_is_dialog_open = false;
        });
}
void ui::MessageDialogHelper::spawn_error_dialog(const std::string &message)
{
    auto [dialog, box] = create_dialog("Cache cleaner: error", "Cache cleaner: caught an error");

    auto *message_label = Gtk::manage(new Gtk::Label(message));
    message_label->set_halign(Gtk::ALIGN_START);
    box->pack_start(*message_label, false, false, 0);

    auto *submessage_label = Gtk::manage(new Gtk::Label());
    submessage_label->set_halign(Gtk::ALIGN_START);
    box->pack_start(*submessage_label, false, false, 0);

    size_t pos = message.find("json");
    if (pos != std::string::npos)
    {
        submessage_label->set_label(
            "JSON error, watch out for trailing commas in the config, or incorrect JSON format.");
    }

    dialog->show_all();
    dialog->run();
    dialog->hide();

    delete dialog;
}
void ui::MessageDialogHelper::spawn_result_dialog(core::ClearReport &clear_report)
{
    auto [dialog, box] = create_dialog("Cache cleaner: deletion complete", "Cache has been successfully cleaned!");

    if (!clear_report.paths.empty())
    {
        for (const auto &path : clear_report.paths)
        {
            auto *path_label = Gtk::manage(new Gtk::Label(path));
            path_label->set_halign(Gtk::ALIGN_START);
            path_label->set_selectable(false);
            box->add(*path_label);
        }
    }
    if (!clear_report.commands.empty())
    {
        for (const auto &[command, status_code] : clear_report.commands)
        {
            auto *command_label =
                Gtk::manage(new Gtk::Label(command + " (status " + std::to_string(status_code) + ")"));
            command_label->set_halign(Gtk::ALIGN_START);
            command_label->set_selectable(false);
            box->add(*command_label);
        }
    }

    dialog->show_all();
    dialog->run();
    dialog->hide();

    delete dialog;

    utils::ConsoleLogger::export_report_to_json(clear_report);
}
std::tuple<Gtk::Dialog *, Gtk::Box *> ui::MessageDialogHelper::create_dialog(const std::string &dialog_title,
                                                                             const std::string &message)
{
    auto *dialog = new Gtk::Dialog(dialog_title);
    dialog->add_button("OK", Gtk::RESPONSE_OK);
    dialog->set_resizable(false);

    auto *box = dialog->get_content_area();
    box->set_spacing(5);
    ui::UIHelper::set_margin(*box, 10);

    auto *label = Gtk::manage(new Gtk::Label(message));
    label->set_halign(Gtk::ALIGN_START);
    box->pack_start(*label, false, false, 0);

    auto *scroll = Gtk::manage(new Gtk::ScrolledWindow());
    scroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scroll->set_propagate_natural_height(true);

    box->pack_start(*scroll, true, true, 0);

    return std::make_tuple(dialog, box);
}
