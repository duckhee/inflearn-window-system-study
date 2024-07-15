cmake_minimum_required(VERSION 3.25)

project(chapter0301FileInfo LANGUAGES CXX)

message("chapter03-01 file info")

add_executable(chapter0301FileInfo)

target_sources(chapter0301FileInfo PRIVATE src/main.cpp)

target_link_libraries(chapter0301FileInfo PRIVATE ${CMAKE_DL_LIBS})