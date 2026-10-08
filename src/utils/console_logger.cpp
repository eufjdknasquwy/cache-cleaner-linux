#include "console_logger.h"
#include <iomanip>
#include <iostream>
void utils::ConsoleLogger::printmsg(const char *message)
{
    std::cout << message << std::endl;
}
void utils::ConsoleLogger::printmsg(const std::string &message)
{
    std::cout << message << std::endl;
}
void utils::ConsoleLogger::print_help_msg(const int indent, const char *arg, const char *desc)
{
    std::cout << "  " << std::left << std::setw(indent) << arg << "  " << desc << std::endl;
}
