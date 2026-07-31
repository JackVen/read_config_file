#pragma once
#include <string>
#include <fstream>
#include <iostream>

class ConfigFileManager
{
private:
    std::string const pathToFile = "../tempSensor.json";

public:
    ConfigFileManager();
    std::string readFile() const;
};