file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
add_library(glad STATIC ${SRC})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)