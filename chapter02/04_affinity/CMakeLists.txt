cmake_minimum_required(VERSION 3.25)

project(chapter0204AffinityMaskSpecificCoreUsing LANGUAGES CXX)

message("chapter02-04 affinity using specific core(친화력)")

add_executable(chapter0204AffinityMaskSpecificCoreUsing)

target_sources(chapter0204AffinityMaskSpecificCoreUsing PRIVATE src/main.cpp)

target_link_libraries(chapter0204AffinityMaskSpecificCoreUsing PRIVATE ${CMAKE_DL_LIBS})