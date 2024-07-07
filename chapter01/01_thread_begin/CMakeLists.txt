cmake_minimum_required(VERSION 3.25)

project(threadBegin LANGUAGES CXX)

message("chapter01-01 thread create example")

add_executable(threadBegin)

target_sources(threadBegin PRIVATE src/01_thread_begin.cpp)