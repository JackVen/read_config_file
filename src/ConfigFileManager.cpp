#include "ConfigFileManager.hpp"

ConfigFileManager::ConfigFileManager() = default;

/*
Reads temp sensor's configuration file:
- Validates the path
- Parses the file in a map
*/
// TODO:
//  1. Trim leading/trailing whitespace.
//    2. Ignore empty lines.
//    3. Ignore comment lines (lines starting with # or ;).
//    4. Strip inline comments (everything after # on a line like cal_ts1 = 0x1FFFF7B8 # memory address).
//    5. Split by = to separate the key and value.
std::string ConfigFileManager::readFile() const
{
    std::ifstream configFile(pathToFile);
    if (!configFile.is_open())
    {
        std::cerr << "Error: Unable to open file at " << pathToFile << '\n';
        return "";
    }

    std::string content;
    std::string line;
    while (std::getline(configFile, line))
    {

        content += line + "\n";
    }

    return content;
}