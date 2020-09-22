set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC ./*.cpp)

add_executable(rubiks_cube ${SRC})

target_include_directories(rubiks_cube PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(rubiks_cube render_sandbox)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
