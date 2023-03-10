project(cg_file_manager)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_library(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/file_manager.h
    file_manager.c
)