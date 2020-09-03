cmake_minimum_required(VERSION 3.17)
project(rubicks_cube)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(rubicks_cube main.cpp)

target_link_libraries(rubicks_cube rubicks_cube_src)