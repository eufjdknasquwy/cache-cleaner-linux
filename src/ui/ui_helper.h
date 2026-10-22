#include <gtkmm.h>
namespace ui
{
    class UIHelper
    {
        public:
            UIHelper() = delete;
            static void clear_box(Gtk::Box &box);
            static void set_margin(Gtk::Widget &widget, int size);
            static void set_button_icon(Gtk::Button &button, const std::string &icon_name, const int icon_size);
    };
} // namespace ui
