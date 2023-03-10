cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)

add_subdirectory(src/containers)
add_subdirectory(src/graphics)
add_subdirectory(src/file_manager)
add_subdirectory(src/log)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/graphics
    ${PROJECT_SOURCE_DIR}/src/containers
    ${PROJECT_SOURCE_DIR}/include)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
    src/main.c
)

target_link_libraries(${PROJECT_NAME} 
    cg_containers
    cg_graphics
    cg_file_manager
    cg_log
)