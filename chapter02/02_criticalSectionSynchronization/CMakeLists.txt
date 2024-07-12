cmake_minimum_required(VERSION 3.25)

project(CriticalSectionSynchronizationError LANGUAGES CXX)

message("chapter02-02 critical section synchronization failed example")

add_executable(CriticalSectionSynchronizationError)

target_sources(CriticalSectionSynchronizationError PRIVATE src/main.cpp)

