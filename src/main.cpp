#include "ui/main_window.h"
#include "utils/console_logger.h"
#include "utils/system_detector.h"
int main(int argc, char *argv[])
{
    utils::ArgsParser::parse(argc, argv);
    core::ConfigLoader::load_config();

    utils::ConsoleLogger::init();
    utils::SystemDetector::list_detections();

    auto app = Gtk::Application::create("org.example.cache-cleaner");

    ui::MainWindow main_window;

    utils::ConsoleLogger::log(core::LogLevel::Info, "Cache cleaner started");

    if (!utils::ArgsParser::tray)
        main_window.show_window();
    return app->run(main_window.get_window());
}
