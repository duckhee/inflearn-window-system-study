cmake_minimum_required(VERSION 3.25)

project(chapter0401ProcessHeapCheck LANGUAGES CXX)

message("chapter04-01 process heap check")

add_executable(chapter0401ProcessHeapCheck)

target_sources(chapter0401ProcessHeapCheck PRIVATE
        src/main.cpp
)