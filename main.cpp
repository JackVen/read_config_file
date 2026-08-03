#include <iostream>
#include "ConfigFileManager.hpp"

int main()
{
    ConfigFileManager cfgm;
    nlohmann::json configFile = cfgm.readFile();
    cfgm.validateFile(configFile);
    cfgm.parseFile(configFile);
    return 0;
}