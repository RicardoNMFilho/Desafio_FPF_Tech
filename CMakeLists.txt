cmake_minimum_required(VERSION 3.10)
project(Desafio_FPF_Tech)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Lib
add_library(gentexts SHARED lib/lib_gentexts.c)
target_include_directories(gentexts PUBLIC lib)

# Backend
add_library(backend_lib STATIC backend/backend.cpp)
target_include_directories(backend_lib PUBLIC backend lib)
target_link_libraries(backend_lib PUBLIC gentexts curl)

# Frontend
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
add_executable(frontend frontend/main.cpp frontend/gui.cpp)
target_include_directories(frontend PUBLIC frontend backend lib)
target_link_libraries(frontend PRIVATE backend_lib Qt5::Widgets Qt5::Core curl)