cmake_minimum_required(VERSION 3.25)

project(chapter0302FileCopy LANGUAGES CXX)

message("chapter03-02 file copy basic file HANDLE using win32 API")

add_executable(chapter0302FileCopy)

target_sources(chapter0302FileCopy PRIVATE src/main.cpp)

target_link_libraries(chapter0302FileCopy PRIVATE ${CMAKE_DL_LIBS})