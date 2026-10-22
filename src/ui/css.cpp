#include "ui/css.h"
#include "core/constants.h"
#include "core/enums.h"
#include "utils/console_logger.h"
#include <gtkmm.h>
void ui::Css::apply_css()
{
    auto css_provider = Gtk::CssProvider::create();
    try
    {
        css_provider->load_from_path(core::get_css_style_path().string());
    }
    catch (const Glib::Error &e)
    {
        utils::ConsoleLogger::log(core::LogLevel::Error, "Failed to load CSS style: ");
        utils::ConsoleLogger::log(core::LogLevel::Error, "Message: " + e.what());
    }

    Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), css_provider,
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
