cmake_minimum_required(VERSION 3.25)

project(chapter0601UsingDLLProgram LANGUAGES CXX VERSION 0.0.1)

message("chapter06-01 dll program using example")

add_executable(chapter0601UsingDLLProgram)

target_link_libraries(chapter0601UsingDLLProgram LINK_PRIVATE
        chapter0601SampleDLL
)

target_sources(chapter0601UsingDLLProgram PRIVATE
    src/main.cpp
)