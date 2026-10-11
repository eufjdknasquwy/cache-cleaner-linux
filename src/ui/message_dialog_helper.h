#pragma once
#include "core/cleaner.h"
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/listbox.h>
namespace ui
{
    class MessageDialogHelper
    {
        public:
            MessageDialogHelper() = delete;
            static void spawn_result_dialog(core::CacheCleaner::ClearReport &del_result);
            static void spawn_error_dialog();

        private:
            static std::tuple<Gtk::Dialog *, Gtk::Box *, Gtk::ListBox *> create_dialog(const std::string &dialog_title,
                                                                                       const std::string &message);
    };
} // namespace ui
