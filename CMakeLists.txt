cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

set (USE_TEST_EXE ON)

set (LIB_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_lib)
set (EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)

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

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/exe)

add_library(${LIB_OUTPUT_NAME} STATIC
    ${SOURCES}
)

target_link_libraries(${LIB_OUTPUT_NAME} PUBLIC
    ${libs}
)
if (USE_TEST_EXE)
    add_executable(${EXE_OUTPUT_NAME}
        src/main.c
    )
    target_link_libraries(${EXE_OUTPUT_NAME}
        ${LIB_OUTPUT_NAME}
    )
endif()
