cmake_minimum_required(VERSION 3.14)
project(ReadConfigFile)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
)
target_include_directories(ReadConfigFileLib PUBLIC inc)
target_link_libraries(ReadConfigFileLib PUBLIC nlohmann_json::nlohmann_json)

# Main application executable
add_executable(ReadFile main.cpp)
target_link_libraries(ReadFile PRIVATE ReadConfigFileLib)

# Testing setup
enable_testing()
include(GoogleTest)

add_executable(hello_test tests/hello_test.cc)
target_link_libraries(hello_test PRIVATE
    ReadConfigFileLib
    GTest::gtest_main
)

gtest_discover_tests(hello_test)

