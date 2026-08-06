#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <Logger.hpp>

static const std::vector<std::string> reqSensorParams = {"sensor_model",
                                                         "operational_max_temp",
                                                         "operational_min_temp",
                                                         "cal_ts1",
                                                         "cal_ts2",
                                                         "adc_resolution",
                                                         "adc_data_aligment"};

class ConfigFileManager
{
private:
    std::string const pathToFile = "tempSensor.json";
    static constexpr std::size_t configFileSize = 8;

public:
    ConfigFileManager();
    nlohmann::json readFile() const;
    bool validateFile(nlohmann::json &file) const;
    void parseFile(nlohmann::json &file) const;
};