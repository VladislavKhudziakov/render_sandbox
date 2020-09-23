
file(GLOB SRC ./*.cpp)

add_executable(raytracer ${SRC})

target_include_directories(raytracer PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(raytracer render_sandbox)