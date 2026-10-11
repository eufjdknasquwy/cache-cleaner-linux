#include "message_dialog_helper.h"
#include "ui_helper.h"
void ui::MessageDialogHelper::spawn_result_dialog(core::CacheCleaner::ClearReport &del_result)
{
    Gtk::Dialog dialog("Cache cleaner: deletion complete", true);
    dialog.add_button("OK", Gtk::RESPONSE_OK);
    dialog.set_resizable(false);

    auto *box = dialog.get_content_area();
    box->set_spacing(5);
    ui::UIHelper::set_margin(*box, 10);

    auto *label = Gtk::manage(
        new Gtk::Label("Cache has been successfully cleaned!\nYou can restore them in your user trash bin"));
    label->set_halign(Gtk::ALIGN_START);
    box->pack_start(*label, false, false, 0);

    auto *scroll = Gtk::manage(new Gtk::ScrolledWindow());
    scroll->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    scroll->set_propagate_natural_height(true);

    auto *listbox = Gtk::manage(new Gtk::ListBox());
    listbox->set_selection_mode(Gtk::SELECTION_NONE);

    if (!del_result.paths.empty())
    {
        for (const auto &path : del_result.paths)
        {
            auto *row = Gtk::manage(new Gtk::ListBoxRow());
            auto *path_label = Gtk::manage(new Gtk::Label(path));
            path_label->set_halign(Gtk::ALIGN_START);
            row->add(*path_label);
            listbox->add(*row);
        }
    }
    if (!del_result.commands.empty())
    {
        for (const auto &[command, status_code] : del_result.commands)
        {
            auto *row = Gtk::manage(new Gtk::ListBoxRow());
            auto *path_label = Gtk::manage(new Gtk::Label(command + " : " + std::to_string(status_code)));
            path_label->set_halign(Gtk::ALIGN_START);
            row->add(*path_label);
            listbox->add(*row);
        }
    }

    scroll->add(*listbox);
    box->pack_start(*scroll, true, true, 0);

    dialog.show_all();
    dialog.run();
}
