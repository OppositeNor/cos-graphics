cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C)

message("Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DCG_DEBUG)
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# set this to OFF if you only want to export the library
set (USE_TEST_EXE ON)

# set this to OFF if you don't want to export the library
set (USE_SHARED_LIB ON)

# use utf-16 little endian to read files
# NOTE: the data stored will be still stored with wide character. In some systems (such as Linux) wide characters are 
# stored as 4 bytes instead of 2 (in Windows).
set (USE_UTF16LE ON)

set (LIB_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_lib)
set (EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)

# Turn this to OFF if you want to print out more information about the resource build process.
set(CGRW_NO_VERBOSE OFF)

# add your resource by appending them to cg_resources list. The default "resource.cgures" should not be included in the list.
# example:
# list(APPEND CG_RESOURCES image_resource.cgures)


set_property(GLOBAL PROPERTY CG_RESOURCES ${CG_RESOURCES})
add_subdirectory(src/graphics)
add_subdirectory(src/log)
add_subdirectory(src/resource)
add_subdirectory(src/utils)

if (USE_TEST_EXE)
    list(APPEND CG_RESOURCES image_resource.cgures)
endif()
add_subdirectory(dependencies/cos-graphics-resource-wrapper)


if (USE_WCHAR)
    add_definitions(-DCG_USE_WCHAR)
endif()

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
        list(APPEND CG_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/64/include")
        if (MINGW)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/64/lib-mingw-w64/libglfw3.a")
        elseif (MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/64/lib-vc2022/glfw3.lib")
        endif()
    else()
        message("Building for: x86")
        list(APPEND CG_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/32/include")
        if (MINGW)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/32/lib-mingw-w64/libglfw3.a")
        elseif (MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            list(APPEND libs "${CMAKE_SOURCE_DIR}/dependencies/glfw/windows/32/lib-vc2022/glfw3.lib")
        endif()
    endif()
endif()

if (USE_UTF16LE MATCHES ON)
    add_definitions(-DCG_USE_UTF16LE)
endif()

include_directories(
    ${CG_INCLUDE_DIRECTORIES}
)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/exe/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

if (USE_SHARED_LIB)
    add_library(${LIB_OUTPUT_NAME} STATIC
        ${SOURCES}
    )

    target_link_libraries(${LIB_OUTPUT_NAME} PUBLIC
        ${libs}
    )
endif()

if (USE_TEST_EXE)
    add_executable(${EXE_OUTPUT_NAME}
        src/main.c
    )
    target_link_libraries(${EXE_OUTPUT_NAME}
        ${LIB_OUTPUT_NAME}
    )
endif()
