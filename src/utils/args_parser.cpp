#include "args_parser.h"
#include "console_logger.h"
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
            std::exit(DEFAULT_HELP_EXIT_CODE);
        }
        else if (arg == "-t" || arg == "--tray")
        {
            tray = true;
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
    utils::ConsoleLogger::print_help_msg(HELP_FLAG_WIDTH, "-t, --tray", "Launch program in tray");

    // конец
    utils::ConsoleLogger::printmsg("");
}
