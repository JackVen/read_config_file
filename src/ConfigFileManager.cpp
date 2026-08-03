#include "ConfigFileManager.hpp"

ConfigFileManager::ConfigFileManager() = default;

/*Reads temp sensor's configuration file:
- Validates the path
- Returns the file as json
- On error returns an empty json
*/
nlohmann::json ConfigFileManager::readFile() const
{
    std::ifstream configFile(pathToFile);
    nlohmann::json configRoot;
    if (!configFile.is_open())
    {
        std::cerr << "Error: Unable to open file at " << pathToFile << '\n';
        return nlohmann::json{};
    }
    try
    {
        configFile >> configRoot;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return configRoot;
}

/* Funtion that validates the config file as follow:
- Check if the file is an valid json object
- Check the size of the file
- Validate the required parameters inside the file
*/
bool ConfigFileManager::validateFile(nlohmann::json &file) const
{
    if (!file.is_object())
    {
        std::cerr << "Validation Error: JSON is not a valid object.\n";
        return false;
    }

    if (file.size() > configFileSize)
    {
        std::cerr << "File size not valid: " << file.size() << std::endl;
        return false;
    }

    for (const auto &param : reqSensorParams)
    {
        if (!file.contains(param))
        {
            std::cout << "Required Key missing: " << param << std::endl;
            return false;
        }
    }

    return true;
}

void ConfigFileManager::parseFile(nlohmann::json &file) const
{

    try
    {
        // Direct access to the parameters
        auto sensorModel = file.at("sensor_model").get<std::string>();
        auto opMaxTemp = std::stoi(file.at("operational_max_temp").get<std::string>());
        auto opMinTemo = std::stoi(file.at("operational_min_temp").get<std::string>());
        auto ts1CalibValue = std::stoi(file.at("cal_ts1").get<std::string>(), nullptr, 16);
        auto ts2CalibValue = std::stoi(file.at("cal_ts2").get<std::string>(), nullptr, 16);
        auto adcResolution = std::stoi(file.at("adc_resolution").get<std::string>());
        auto adcDataAlig = file.at("adc_data_aligment").get<std::string>();

        // Print the parameters as primitive data types
        std::cout << "sensorModel : " << sensorModel << std::endl;
        std::cout << "opMaxTemp : " << opMaxTemp << std::endl;
        std::cout << "opMinTemo : " << opMinTemo << std::endl;
        std::cout << "ts1CalibValue : " << ts1CalibValue << std::endl;
        std::cout << "ts2CalibValue : " << ts2CalibValue << std::endl;
        std::cout << "adcResolution : " << adcResolution << std::endl;
        std::cout << "adcDataAlig : " << adcDataAlig << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error when parsing config file: " << e.what() << std::endl;
    }
}