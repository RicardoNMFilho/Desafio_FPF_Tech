cmake_minimum_required(VERSION 3.10)

project(backend)

add_library(gentexts SHARED ../lib/lib_gentexts.c)
target_include_directories(gentexts PUBLIC ../lib)

add_executable(backend backend.cpp)

target_include_directories(backend PRIVATE ../lib)

target_link_libraries(backend PRIVATE gentexts dl curl)