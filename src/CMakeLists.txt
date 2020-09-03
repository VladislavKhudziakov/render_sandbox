file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(rubicks_cube_src STATIC ${SRC})
target_link_libraries(rubicks_cube_src PUBLIC glad glfw)

target_include_directories(rubicks_cube_src PUBLIC ${CMAKE_CURRENT_LIST_DIR})
