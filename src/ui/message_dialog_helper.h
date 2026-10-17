#pragma once
#include "core/structs.h"
#include <gtkmm.h>
namespace ui
{
    class MessageDialogHelper
    {
        public:
            MessageDialogHelper() = delete;
            static void spawn_result_dialog(core::ClearReport &clear_report);
            static void spawn_error_dialog();

        private:
            static std::tuple<Gtk::Dialog *, Gtk::Box *, Gtk::ListBox *> create_dialog(const std::string &dialog_title,
                                                                                       const std::string &message);
    };
} // namespace ui
