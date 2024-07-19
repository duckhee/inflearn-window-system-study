cmake_minimum_required(VERSION 3.25)

project(chapter0303FileAsyncEvent LANGUAGES CXX)

message("chapter03-03 file async event create test")

add_executable(chapter0303FileAsyncEvent)

target_sources(chapter0303FileAsyncEvent PRIVATE src/main.cpp)

