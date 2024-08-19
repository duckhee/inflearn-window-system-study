cmake_minimum_required(VERSION 3.25)

project(chapter0501ExecuteProcessList LANGUAGES CXX)

message("chapter05-01 execute process list")

add_executable(chapter0501ExecuteProcessList)

target_sources(chapter0501ExecuteProcessList PRIVATE
        src/main.cpp
)