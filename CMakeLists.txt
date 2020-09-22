cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

project(render_sandbox)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/samples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third)

set(CMAKE_CXX_STANDARD 20)