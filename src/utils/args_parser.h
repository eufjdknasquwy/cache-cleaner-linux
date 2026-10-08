namespace utils
{
    class ArgsParser
    {
        public:
            ArgsParser() = delete;
            static void parse(int &argc, char *argv[]);
            static void print_help();

            static bool tray;

        private:
            static constexpr int HELP_FLAG_WIDTH = 20;
            static constexpr int EXIT_CODE_NO_ERRS = 0;
            static constexpr int EXIT_CODE_ERRS = 1;
    };
} // namespace utils
