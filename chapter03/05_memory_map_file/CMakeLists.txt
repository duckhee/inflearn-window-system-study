cmake_minimum_required(VERSION 3.25)

project(chapter0305MemoryMapFile LANGUAGES CXX)

message("chapter03-05 memory map file")

add_executable(chapter0305MemoryMapFile)

target_sources(chapter0305MemoryMapFile PRIVATE src/main.cpp)