cmake_minimum_required(VERSION 3.25)

project(beginthreadex LANGUAGES CXX)

message("chapter01-02 create thread using _beginthreadex function example")

add_executable(beginthreadex)

target_sources(beginthreadex PRIVATE ./src/02_beginthreadex.cpp)