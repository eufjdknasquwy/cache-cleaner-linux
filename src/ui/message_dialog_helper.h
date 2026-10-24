#pragma once
#include "core/structs.h"
#include <gtkmm.h>
namespace ui
{
    class MessageDialogHelper
    {
        public:
            MessageDialogHelper() = delete;

            static Glib::Dispatcher s_error_dispatcher;
            static std::string s_pending_error;
            static std::mutex s_error_mutex;
            static std::atomic<bool> s_is_dialog_open;

            static void init_error_handler();
            static void spawn_result_dialog(core::ClearReport &clear_report);
            static void spawn_error_dialog(const std::string &message);

        private:
            static std::tuple<Gtk::Dialog *, Gtk::Box *> create_dialog(const std::string &dialog_title,
                                                                       const std::string &message);
    };
} // namespace ui
