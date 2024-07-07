cmake_minimum_required(VERSION 3.25)

project(threadControl LANGUAGES CXX)

message("chapter01-03 thread control example")

add_executable(threadControl)

target_sources(threadControl PRIVATE src/threadControl.cpp)