add_library(backend_lib backend.cpp)
target_include_directories(backend_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backend_lib PUBLIC gentexts)