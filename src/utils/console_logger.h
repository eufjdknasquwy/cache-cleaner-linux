#include "core/enums.h"
#include "core/structs.h"
#include <fstream>
namespace utils
{
    class ConsoleLogger
    {
        public:
            ConsoleLogger() = delete;
            static void init();

            static void printmsg(const std::string_view message);
            static void print_help_msg(const int indent, const char *arg, const char *desc);

            static void log(const core::LogLevel level, const std::string_view message);
            static void flush_errors_to_ui();
            static void reset_error_buffer();
            static void export_report_to_json(core::ClearReport &clear_report);

        private:
            static constexpr const char *DEFAULT_TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";
            static constexpr const char *GREEN = "\033[32m";
            static constexpr const char *YELLOW = "\033[33m";
            static constexpr const char *RED = "\033[91m";
            static constexpr const char *RESET = "\033[0m";

            static std::string s_home_path;
            static std::string s_error_buffer;
            static std::ofstream s_log;

            static std::string timestamp(const std::string format);
            static std::string level_to_str(const core::LogLevel level);
    };
} // namespace utils
