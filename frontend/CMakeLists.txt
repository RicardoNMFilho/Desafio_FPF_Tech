project(frontend)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(frontend main.cpp)

target_include_directories(frontend PRIVATE ../backend ../lib)
target_link_libraries(frontend PRIVATE backend_lib gentexts Qt5::Widgets curl)