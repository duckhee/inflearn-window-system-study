cmake_minimum_required(VERSION 3.25)

project(chapter0203CreateSemaphore LANGUAGES CXX)

message("chapter02-03 create semaphore")

add_executable(chapter0203CreateSemaphore)

target_sources(chapter0203CreateSemaphore PRIVATE src/main.cpp)