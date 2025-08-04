cmake_minimum_required(VERSION 3.10)

project(backend)

add_executable(backend backend.cpp)

target_link_libraries(backend dl curl)

target_include_directories(backend PRIVATE ../lib)