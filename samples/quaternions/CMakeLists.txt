file(GLOB SRC ./*.cpp)

add_executable(quat ${SRC})

target_include_directories(quat PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(quat render_sandbox)