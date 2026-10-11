#include "console_logger.h"
#include "core/enums.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
namespace fs = std::filesystem;
std::ofstream utils::ConsoleLogger::s_file;
void utils::ConsoleLogger::init(const std::string &log_path)
{
    fs::path p(log_path);
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);

    s_file.open(log_path, std::ios::app);
}
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
void utils::ConsoleLogger::log(core::LogLevel level, const std::string &msg)
{
    std::string full_msg = "[" + timestamp() + "] " + "[" + level_to_str(level) + "] " + msg;
    if (level != core::LogLevel::Error)
        std::cout << full_msg << std::endl;
    else
        std::cerr << full_msg << std::endl;

    if (s_file.is_open())
    {
        s_file << full_msg << "\n";
        s_file.flush();
    }
}
std::string utils::ConsoleLogger::timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}
std::string utils::ConsoleLogger::level_to_str(core::LogLevel level)
{
    switch (level)
    {
    case core::LogLevel::Info:
        return "INFO";
    case core::LogLevel::Warning:
        return "WARNING";
    case core::LogLevel::Error:
        return "ERROR";
    }
    return "UNKNOWN";
}
