cmake_minimum_required(VERSION 3.25)

project(chapter0402VirtualMemoryProtect LANGUAGES CXX)

message("chapter04-02 protect virtual memory")

add_executable(chapter0402VirtualMemoryProtect)

target_sources(chapter0402VirtualMemoryProtect PRIVATE
        src/main.cpp
)