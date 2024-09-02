cmake_minimum_required(VERSION 3.25)

project(chapter0601SampleDLL LANGUAGES CXX VERSION 0.0.1)

message("chapter06-01 DLL Sample Project")

add_library(chapter0601SampleDLL)

target_include_directories(chapter0601SampleDLL PRIVATE
        header
)

target_sources(chapter0601SampleDLL PRIVATE
        src/dllmain.cpp
        src/pch.cpp
        header/framework.h
        header/pch.h
)