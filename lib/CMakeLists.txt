cmake_minimum_required(VERSION 3.10)

project(libgentexts C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(gentexts SHARED lib_gentexts.c)

target_include_directories(gentexts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})