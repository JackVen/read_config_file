#include "Logger.hpp"

/* Logger (constructor)
Opens the log file at object construction
Param: Log file name
*/
Logger::Logger(const std::string &fileName)
{
    logFile.open(fileName, std::ios::app);
    if (!logFile.is_open())
    {
        std::cerr << "Log file couldn't open" << std::endl;
    }
}
/*Log function
- Gets the current time
- Creates a log entry
- write the log entry to file
*/
void Logger::log(LogLevel level, std::string_view message)
{

    std::ostringstream logEntry;
    logEntry << "[" << getCurrentTime()
             << "] "
             << levelToString(level) << ": " << message
             << '\n';
    std::string logEnt = logEntry.str();
    std::cout << logEnt;
    writeLogEntry(logEnt);
}

/*
Gets the current local time as timestamp
- format date as Y-m-d
- time format as H:M:S
*/
std::string Logger::getCurrentTime()
{
    time_t now = time(0);
    tm *timeinfo = localtime(&now);
    char timeStamp[20];
    strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    return timeStamp;
}

/*Writes log entry into the log file*/
void Logger::writeLogEntry(std::string_view logEntry)
{

    if (logFile.is_open())
    {
        logFile << logEntry;
        logFile
            .flush();
    }
    else
    {
        std::cerr << "Couldn't write to log to file" << std::endl;
    }
}