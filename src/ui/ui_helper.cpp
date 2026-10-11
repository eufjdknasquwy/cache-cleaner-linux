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
