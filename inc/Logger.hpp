#pragma once
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger
{
private:
    std::ofstream logFile;

    Logger();
    ~Logger();

    std::string_view levelToString(LogLevel level) const;
    std::string getCurrentTime();
    void writeLogEntry(std::string_view logEntry);
    void logImpl(LogLevel level, std::string_view message);

public:
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;

    static Logger &getInstance();
    static void log(LogLevel level, std::string_view message);
};