#include "console_logger.h"
#include "core/config_loader.h"
#include "core/enums.h"
#include "ui/message_dialog_helper.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
namespace fs = std::filesystem;
std::string utils::ConsoleLogger::s_home_path = std::getenv("HOME") ? std::getenv("HOME") : "/tmp";
std::string utils::ConsoleLogger::s_error_buffer;
std::ofstream utils::ConsoleLogger::s_log;
void utils::ConsoleLogger::init()
{
    std::string log_path = s_home_path + "/.local/state/cache-cleaner/log-" + timestamp("%Y-%m-%d_%H:%M") + ".txt";

    fs::path log(log_path);
    std::error_code ec;
    fs::create_directories(log.parent_path(), ec);

    s_log.open(log_path);
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
void utils::ConsoleLogger::export_report_to_json(core::ClearReport &clear_report)
{
    std::string report_path =
        s_home_path + "/.local/state/cache-cleaner/reports/clear_report-" + timestamp("%Y-%m-%d_%H:%M:%S") + ".json";

    fs::path report(report_path);
    std::error_code ec;
    fs::create_directories(report.parent_path(), ec);

    std::ofstream report_file;
    report_file.open(report_path);

    if (!report_file.is_open())
    {
        log(core::LogLevel::Error, "Failed to open clear report file");
        return;
    }

    log(core::LogLevel::Info, "Exporting clear report to JSON...");
    core::ConfigLoader::export_report_to_json(report_file, clear_report);
    log(core::LogLevel::Info, "Exporting complete");

    report_file.close();
}
void utils::ConsoleLogger::log(const core::LogLevel level, const std::string &msg)
{
    std::string full_msg = "[" + timestamp("") + "] " + "[" + level_to_str(level) + "] " + msg;

    switch (level)
    {
    case core::LogLevel::Info:
        std::cout << GREEN << full_msg << std::endl;
        break;
    case core::LogLevel::Warning:
        std::cout << YELLOW << full_msg << std::endl;
        break;
    case core::LogLevel::Error:
        std::cout << RED << full_msg << std::endl;
        s_error_buffer += msg + "\n";
        break;
    default:
        std::cout << full_msg << std::endl;
        break;
    }

    if (s_log.is_open())
    {
        s_log << full_msg << "\n";
        s_log.flush();
    }
}
void utils::ConsoleLogger::flush_errors_to_ui()
{
    if (s_error_buffer.empty())
        return;

    {
        std::lock_guard<std::mutex> lock(ui::MessageDialogHelper::s_error_mutex);
        ui::MessageDialogHelper::s_pending_error = s_error_buffer;
    }
    ui::MessageDialogHelper::s_error_dispatcher.emit();

    s_error_buffer.clear();
}
std::string utils::ConsoleLogger::timestamp(std::string format)
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    char buf[32];
    const char *final_format = format.empty() ? DEFAULT_TIMESTAMP_FORMAT : format.c_str();
    std::strftime(buf, sizeof(buf), final_format, &tm);
    return buf;
}
std::string utils::ConsoleLogger::level_to_str(const core::LogLevel level)
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
