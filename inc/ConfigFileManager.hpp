#pragma once
#include <string>

class ConfigFileManager {
private:
    std::string filePath = "";
public:
    ConfigFileManager();
    std::string readFile(std::string file);
};