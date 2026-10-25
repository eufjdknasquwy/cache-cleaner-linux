#pragma once
namespace core
{
    enum class DangerLevel
    {
        Safe,
        Warning,
        Unknown,
        System,
        User
    };
    enum class LogLevel
    {
        Info,
        Warning,
        Error,
        Debug
    };
} // namespace core
