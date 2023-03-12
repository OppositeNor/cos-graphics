cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/export/lib)

set (USE_TEST_EXE ON)

add_subdirectory(src/graphics)
add_subdirectory(src/file_manager)
add_subdirectory(src/log)

# opengl
find_package(OpenGL REQUIRED)
list(APPEND libs OpenGL::GL)

# GLFW
find_package(glfw3 REQUIRED)
list(APPEND libs glfw)

list(APPEND libs m)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/graphics
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/export/bin)

add_library(${PROJECT_NAME}_lib STATIC
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    ${libs}
)
if (USE_TEST_EXE)
    add_executable(${PROJECT_NAME}_demo
        src/main.c
    )
    target_link_libraries(${PROJECT_NAME}_demo
    ${PROJECT_NAME}_lib
    )
endif()
