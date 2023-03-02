project(file_manager)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME}
    file_manager.h
    file_manager.c
)