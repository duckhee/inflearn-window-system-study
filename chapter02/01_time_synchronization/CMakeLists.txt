cmake_minimum_required(VERSION 3.25)

project(UsingEventTimingSynchronization LANGUAGES CXX)

message("chapter02-01 Event timing Synchronization")

add_executable(UsingEventTimingSynchronization)

target_sources(UsingEventTimingSynchronization PRIVATE src/event_timing_synchronzation.cpp)