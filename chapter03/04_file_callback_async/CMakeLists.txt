cmake_minimum_required(VERSION 3.25)

project(chapter0304FileCallbackAsync LANGUAGES CXX)

message("chapter03-04 file async write using callback")

add_executable(chapter0304FileCallbackAsync)

target_sources(chapter0304FileCallbackAsync PRIVATE src/main.cpp)