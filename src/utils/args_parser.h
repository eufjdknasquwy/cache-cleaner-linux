namespace utils
{
    class ArgsParser
    {
        public:
            ArgsParser() = delete;
            static void parse(int &argc, char *argv[]);
            static void print_help();

            static bool tray;
            static bool dry_run;
            static bool yes;

        private:
            static constexpr int HELP_FLAG_WIDTH = 40;
            static constexpr int EXIT_CODE_NO_ERRS = 0;
            static constexpr int EXIT_CODE_ERRS = 1;

            static std::vector<std::string> actions;
    };
} // namespace utils
