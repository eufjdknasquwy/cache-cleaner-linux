#include "ui/main_window.h"
int main(int argc, char *argv[])
{
    utils::ArgsParser::parse(argc, argv);
    core::ConfigLoader::load_config();

    auto app = Gtk::Application::create("org.example.cache-cleaner");

    ui::MainWindow main_window;
    if (!utils::ArgsParser::tray)
        main_window.show_window();
    return app->run(main_window.get_window());
}
