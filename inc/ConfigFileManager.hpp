#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class ConfigFileManager
{
private:
    std::string const pathToFile = "../tempSenor.json";

public:
    ConfigFileManager();
    nlohmann::json readFile() const;
};