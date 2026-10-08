#pragma once
#include "danger_level.h"
namespace core::cli
{
    class CliTools
    {
        public:
            CliTools() = delete;
            static void delete_category(core::DangerLevel danger);
            static void list_category(core::DangerLevel danger);
            static std::vector<std::string> list_all_categories();

        private:
            static bool ask_confirmation(const char *prompt);
    };
} // namespace core::cli
