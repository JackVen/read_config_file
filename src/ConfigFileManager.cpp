#include "ConfigFileManager.hpp"

ConfigFileManager::ConfigFileManager() = default;

/*Reads temp sensor's configuration file:
- Validates the path
- Returns the file as json*/
nlohmann::json ConfigFileManager::readFile() const
{
    std::ifstream configFile(pathToFile);
    if (!configFile.is_open())
    {
        std::cerr << "Error: Unable to open file at " << pathToFile << '\n';
        return nlohmann::json{};
    }

    nlohmann::json configRoot;
    configFile >> configRoot;

    return configRoot;
}