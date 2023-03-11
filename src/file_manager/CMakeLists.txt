project(cg_file_manager)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/file_manager.h
    file_manager.c
)