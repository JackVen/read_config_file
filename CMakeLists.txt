cmake_minimum_required(VERSION 3.10)
project(ReadConfigFile)

add_executable(ReadFile main.cpp src/ConfigFileManager.cpp)
target_include_directories(ReadFile PRIVATE inc)