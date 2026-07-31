#include <iostream>
#include "ConfigFileManager.hpp" 



int main (){
    ConfigFileManager cfgm;
    std::cout<< cfgm.readFile() << std::endl;

    return 0;
}