cmake_minimum_required(VERSION 3.14)
project(ReadConfigFile)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

add_executable(ReadFile main.cpp src/ConfigFileManager.cpp src/Logger.cpp)
target_include_directories(ReadFile PRIVATE inc)
target_link_libraries(ReadFile PRIVATE nlohmann_json::nlohmann_json)