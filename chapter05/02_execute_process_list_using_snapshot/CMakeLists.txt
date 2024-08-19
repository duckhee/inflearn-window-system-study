cmake_minimum_required(VERSION 3.25)

project(chapter0502ExecuteProcessListUsingSnapShot LANGUAGES CXX)

message("chapter05-02 execute process list using snapshot")

add_executable(chapter0502ExecuteProcessListUsingSnapShot)

target_sources(chapter0502ExecuteProcessListUsingSnapShot PRIVATE
        src/main.cpp
)