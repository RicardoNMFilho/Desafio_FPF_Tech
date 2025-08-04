add_library(backend_lib STATIC backend.cpp)
target_include_directories(backend_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})