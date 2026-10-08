#include "args_parser.h"
#include "console_logger.h"
#include "core/cli_tools.h"
#include <string>
bool utils::ArgsParser::tray = false;
void utils::ArgsParser::parse(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            print_help();
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "-v" || arg == "--version")
        {
            utils::ConsoleLogger::printmsg(std::string(PROJECT_NAME) + " v" + std::string(PROJECT_VERSION));
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "-t" || arg == "--tray")
        {
            tray = true;
        }
        else if (arg == "-l" || arg == "--list")
        {
            std::vector<std::string> all_paths = core::cli::CliTools::list_all_categories();
            for (std::string path : all_paths)
            {
                utils::ConsoleLogger::printmsg(path);
            }
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "--delete-safe")
        {
            core::cli::CliTools::delete_category(core::DangerLevel::Safe);
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "--delete-warning")
        {
            core::cli::CliTools::delete_category(core::DangerLevel::Warning);
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "--delete-unknown")
        {
            core::cli::CliTools::delete_category(core::DangerLevel::Unknown);
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "--delete-system")
        {
            core::cli::CliTools::delete_category(core::DangerLevel::System);
            std::exit(EXIT_CODE_NO_ERRS);
        }
        else if (arg == "--delete-user")
        {
            core::cli::CliTools::delete_category(core::DangerLevel::User);
            std::exit(EXIT_CODE_NO_ERRS);
        }
    }
}
void utils::ArgsParser::print_help()
{
    // начало
    utils::ConsoleLogger::printmsg("Usage: cache-cleaner [OPTION...]");
    utils::ConsoleLogger::printmsg("");
    utils::ConsoleLogger::printmsg("Options:");

    // аргументы с описаниями
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "-h, --help", "Show help message");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "-v, --version", "Show the cache cleaner version");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "-t, --tray", "Launch program in tray");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "-l, --list", "List available to delete caches");

    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "--delete-safe", "Delete cache in safe category");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "--delete-warning", "Delete cache in warning category");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "--delete-unknown", "Delete cache in unknown category");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "--delete-system", "Delete cache in system category");
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "--delete-user", "Delete cache in user category");

    // конец
    utils::ConsoleLogger::printmsg("");
}
