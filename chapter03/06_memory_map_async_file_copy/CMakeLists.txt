cmake_minimum_required(VERSION 3.25)

project(chapter0306MemoryMapCopyAsync LANGUAGES CXX)

message("chapter03-06 memory map copy async")

add_executable(chapter0306MemoryMapCopyAsync)

target_sources(
        chapter0306MemoryMapCopyAsync PRIVATE
        src/main.cpp
)