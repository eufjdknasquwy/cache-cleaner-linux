#include <gtkmm.h>
namespace ui
{
    class UIHelper
    {
        public:
            UIHelper() = delete;
            static void clear_box(Gtk::Box &box);
            static void set_margin(Gtk::Widget &widget, int size);

        private:
    };
} // namespace ui
