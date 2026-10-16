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

            static void printmsg(const char *message);
            static void printmsg(const std::string &message);
            static void print_help_msg(const int indent, const char *arg, const char *desc);

            static void log(const core::LogLevel level, const std::string &message);
            static void export_report_to_json(core::ClearReport &clear_report);

        private:
            static constexpr const char *DEFAULT_TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";

            static std::string s_home_path;

            static std::ofstream s_log;

            static std::string timestamp(std::string format);
            static std::string level_to_str(const core::LogLevel level);
    };
} // namespace utils
