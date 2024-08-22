cmake_minimum_required(VERSION 3.25)

project(chapter0504SharedMemoryIPC_Server LANGUAGES CXX)

message("chapter05-04 shared memory IPC Server")

add_executable(chapter0504SharedMemoryIPC_Server)


target_sources(chapter0504SharedMemoryIPC_Server PRIVATE
        src/main.cpp
)