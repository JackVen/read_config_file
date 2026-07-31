#include <iostream>
#include "ConfigFileManager.hpp"

int main()
{
    ConfigFileManager cfgm;
    nlohmann::json config = cfgm.readFile();
    std::cout << config.dump(4) << std::endl;

    return 0;
}