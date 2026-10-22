#include "ui_helper.h"
void ui::UIHelper::clear_box(Gtk::Box &box)
{
    auto children = box.get_children();
    for (auto *child : children)
    {
        box.remove(*child);
    }
}
void ui::UIHelper::set_margin(Gtk::Widget &widget, int size)
{
    widget.set_margin_top(size);
    widget.set_margin_bottom(size);
    widget.set_margin_start(size);
    widget.set_margin_end(size);
}
void ui::UIHelper::set_button_icon(Gtk::Button &button, const std::string &icon_name, const int icon_size)
{
    auto *icon = Gtk::manage(new Gtk::Image());
    icon->set_from_icon_name(icon_name, Gtk::ICON_SIZE_DIALOG);
    icon->set_pixel_size(icon_size);
    button.set_image(*icon);
    button.set_always_show_image(true);
}
