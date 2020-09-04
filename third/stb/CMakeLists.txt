add_library(stb_image INTERFACE)
add_dependencies(stb_image ${CMAKE_CURRENT_LIST_DIR}/stb_image.h)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_LIST_DIR})