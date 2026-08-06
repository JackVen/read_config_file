#include "ConfigFileManager.hpp"

ConfigFileManager::ConfigFileManager() = default;

/*Reads temp sensor's configuration file:
- Validates the path
- Returns the file as json
- On error returns an empty json
*/
nlohmann::json ConfigFileManager::readFile() const
{
    Logger::log(LogLevel::INFO, "Reading configuration file");

    std::ifstream configFile(pathToFile);
    nlohmann::json configRoot;
    if (!configFile.is_open())
    {
        Logger::log(LogLevel::ERROR, "Unable to open configuration file: " + pathToFile);
        return nlohmann::json{};
    }
    try
    {
        configFile >> configRoot;
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, std::string("JSON parse error: ") + e.what());
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
    Logger::log(LogLevel::INFO, "Validating configuration file");

    if (!file.is_object())
    {
        Logger::log(LogLevel::ERROR, "Validation Error: JSON is not a valid object.");
        return false;
    }

    if (file.size() > configFileSize)
    {
        Logger::log(LogLevel::ERROR, "File size not valid: " + std::to_string(file.size()));
        return false;
    }

    for (const auto &param : reqSensorParams)
    {
        if (!file.contains(param))
        {
            Logger::log(LogLevel::ERROR, "Required Key missing: " + param);
            return false;
        }
    }

    return true;
}

void ConfigFileManager::parseFile(nlohmann::json &file) const
{
    Logger::log(LogLevel::INFO, "Parsing configuration file");

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
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, std::string("Error when parsing config file: ") + e.what());
    }
}