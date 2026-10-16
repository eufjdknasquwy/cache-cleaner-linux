#pragma once
namespace core
{
    struct ClearReport
    {
            std::vector<std::string> paths;
            std::map<std::string, int> commands;
    };
    struct ClearOptions
    {
            bool dry_run = false;
            bool cli = false;
    };
} // namespace core
