#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

enum LogLevel
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
    ofstream logFile;
    string levelToString(LogLevel level)
    {
        switch (level)
        {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
        }
    }

public:
    // Constructor
    Logger(const string &fileName)
    {
        logFile.open(fileName, ios::app);
        if (!logFile.is_open())
        {
            cerr << "Log file couldn't open" << endl;
        }
    }
    // Destructor
    ~Logger() { logFile.close(); }
    void log(LogLevel level, const string &message);
};