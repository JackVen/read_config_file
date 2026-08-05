#pragma once
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

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
    std::string_view levelToString(LogLevel level) const
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
        }
    }
    std::string getCurrentTime();
    void writeLogEntry(std::string_view logEntry);

public:
    Logger(const std::string &fileName);
    ~Logger() { logFile.close(); }
    // Singleton pattern
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    void log(LogLevel level, std::string_view message);
};