cmake_minimum_required(VERSION 3.25)

project(04_sleep_function LANGUAGES CXX)

message("chapter01-04 sleep function")

add_executable(04_sleep_function)

target_sources(04_sleep_function PRIVATE src/01_sleep_function.cpp)