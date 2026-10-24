#include "core/config_loader.h"
#include "ui/main_window.h"
#include "ui/message_dialog_helper.h"
#include "utils/console_logger.h"
#include "utils/system_detector.h"
#include <thread>
int main(int argc, char *argv[])
{
    utils::ArgsParser::parse(argc, argv);

    utils::ConsoleLogger::init();
    ui::MessageDialogHelper::init_error_handler();

    std::thread load_config_thread(
        []
        {
            try
            {
                core::ConfigLoader::load_config();
            }
            catch (const std::exception &e)
            {
                utils::ConsoleLogger::log(core::LogLevel::Error, ("Config load failed: " + std::string(e.what())));
            }
        });
    std::thread load_pm_config_thread(
        []
        {
            try
            {
                core::ConfigLoader::load_pm_config();
            }
            catch (const std::exception &e)
            {
                utils::ConsoleLogger::log(core::LogLevel::Error,
                                          std::string("Package manager config load failed: ") + e.what());
            }
        });

    load_pm_config_thread.join();
    utils::SystemDetector::list_detections();

    auto app = Gtk::Application::create("org.example.cache-cleaner");

    ui::MainWindow main_window;

    utils::ConsoleLogger::log(core::LogLevel::Info, "Cache cleaner started");

    if (!utils::ArgsParser::tray)
        main_window.show_window();

    load_config_thread.join();

    return app->run(main_window.get_window());
}
