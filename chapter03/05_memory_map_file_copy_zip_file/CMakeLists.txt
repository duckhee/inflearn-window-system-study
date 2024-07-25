cmake_minimum_required(VERSION 3.25)

project(chapter0305MapViewOfFileExample LANGUAGES CXX)

message("chapter03-05 example map view file")

add_executable(chapter0305MapViewOfFileExample)

target_sources(chapter0305MapViewOfFileExample PRIVATE src/main.cpp)