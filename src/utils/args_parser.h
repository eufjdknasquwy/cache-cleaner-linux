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
            static constexpr int DEFAULT_HELP_EXIT_CODE = 0;
    };
} // namespace utils
