cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DCG_DEBUG)
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# set this to OFF if you only want to export the library
set (USE_TEST_EXE ON)

set (LIB_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_lib)
set (EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)

add_subdirectory(src/graphics)
add_subdirectory(src/log)
add_subdirectory(src/resource)

add_subdirectory(dependencies/cos-graphics-resource-wrapper)


# opengl
find_package(OpenGL REQUIRED)
list(APPEND libs OpenGL::GL)

set(CG_INCLUDE_DIRECTORIES 
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include
    ${CMAKE_SOURCE_DIR}/dependencies/stb)

message("System: ${CMAKE_SYSTEM_NAME}")

#linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # GLFW
    find_package(glfw3 REQUIRED)
    list(APPEND libs glfw)

    list(APPEND libs m)
#windows
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message("Building for: x64")
        if (MINGW)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/64/lib-mingw-w64/libglfw3.a")
            list(APPEND CG_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/64/include")
        endif()
    else()
        message("Building for: x86")
        if (MINGW)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/32/lib-mingw-w64/libglfw3.a")
            list(APPEND CG_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/32/include")
        endif()
    endif()
endif()

include_directories(
    ${CG_INCLUDE_DIRECTORIES}
)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/exe/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

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
