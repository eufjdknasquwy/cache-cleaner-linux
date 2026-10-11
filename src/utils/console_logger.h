#include "core/enums.h"
#include <fstream>
namespace utils
{
    class ConsoleLogger
    {
        public:
            ConsoleLogger() = delete;
            static void init(const std::string &log_path);

            static void printmsg(const char *message);
            static void printmsg(const std::string &message);
            static void print_help_msg(const int indent, const char *arg, const char *desc);

            static void log(core::LogLevel level, const std::string &message);

        private:
            static std::ofstream s_file;

            static std::string timestamp();
            static std::string level_to_str(core::LogLevel level);
    };
} // namespace utils
