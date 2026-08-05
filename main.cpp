#include <iostream>
#include "ConfigFileManager.hpp"
#include "Logger.hpp"

int main()
{
    ConfigFileManager cfgm;
    Logger logger("logFile.log");
    logger.log(LogLevel::DEBUG, "INIT");
    nlohmann::json configFile = cfgm.readFile();
    cfgm.validateFile(configFile);
    cfgm.parseFile(configFile);
    return 0;
}