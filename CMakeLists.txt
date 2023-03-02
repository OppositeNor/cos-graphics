cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)

add_subdirectory(src/containers)
add_subdirectory(src/graphics)
add_subdirectory(src/file_manager)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/graphics
    ${PROJECT_SOURCE_DIR}/src/containers
    ${PROJECT_SOURCE_DIR}/dependencies/glad/include)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
    dependencies/glad/src/glad.c

    src/defs.h
    src/main.c

    src/log/log.h
    src/log/log.c
)

target_link_libraries(${PROJECT_NAME} 
    containers
    graphics
    file_manager
)