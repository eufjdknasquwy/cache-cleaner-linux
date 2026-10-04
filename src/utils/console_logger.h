namespace utils
{
    class ConsoleLogger
    {
        public:
            ConsoleLogger() = delete;
            static void printmsg(const char *message);
            static void print_help_msg(const int indent, const char *arg, const char *desc);
    };
} // namespace utils
