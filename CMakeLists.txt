cmake_minimum_required(VERSION 3.14...3.24)
project(ReadConfigFile)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Json repo
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Google test repo
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json googletest)

# Core library target
add_library(ReadConfigFileLib
    src/ConfigFileManager.cpp
    src/Logger.cpp
    src/TemperatureSensor.cpp
    inc/ConfigFileManager.hpp
    inc/Logger.hpp
    inc/TemperatureSensor.hpp
)
target_include_directories(ReadConfigFileLib PUBLIC inc)
target_link_libraries(ReadConfigFileLib PUBLIC nlohmann_json::nlohmann_json)

# Copy configuration file to binary directory for runtime execution
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tempSensor.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Main application executable
add_executable(ReadFile main.cpp)
target_link_libraries(ReadFile PRIVATE ReadConfigFileLib)

# Testing setup
enable_testing()
include(GoogleTest)

add_executable(ConfigFileManagerTests tests/config_manager_test.cc)
target_link_libraries(ConfigFileManagerTests PRIVATE
    ReadConfigFileLib
    GTest::gtest_main
)

gtest_discover_tests(ConfigFileManagerTests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(TemperatureSensorTests tests/temperature_sensor_test.cc)
target_link_libraries(TemperatureSensorTests PRIVATE
    ReadConfigFileLib
    GTest::gtest_main
)

gtest_discover_tests(TemperatureSensorTests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

