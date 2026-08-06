#include <iostream>
#include "ConfigFileManager.hpp"
#include "Logger.hpp"

int main()
{
    Logger::log(LogLevel::INFO, "System Init");
    ConfigFileManager cfgm;
    nlohmann::json configFile = cfgm.readFile();
    cfgm.validateFile(configFile);
    cfgm.parseFile(configFile);
    return 0;
}