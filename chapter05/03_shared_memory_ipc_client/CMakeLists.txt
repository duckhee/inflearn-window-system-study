cmake_minimum_required(VERSION 3.25)

project(chapter0503ShaderMemoryIPC_Client LANGUAGES CXX VERSION 0.0.1)

message("chapter05-03 shared memory client")

add_executable(chapter0503ShaderMemoryIPC_Client)

target_sources(chapter0503ShaderMemoryIPC_Client PRIVATE
        src/main.cpp
)