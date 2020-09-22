file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(render_sandbox STATIC ${SRC})
target_link_libraries(render_sandbox PUBLIC glad glfw stb_image)

target_include_directories(render_sandbox PUBLIC ${CMAKE_CURRENT_LIST_DIR})
