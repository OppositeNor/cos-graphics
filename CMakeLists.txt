cmake_minimum_required(VERSION 3.15)

project(CosGraphics LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set (CMAKE_BUILD_TYPE Release)

message("Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DCG_DEBUG)
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})

# set this to OFF if you only want to export the library
set (CG_USE_TEST_EXE ON)

# set this to OFF if you don't want to export the library
set (CG_EXPORT_LIBRARY ON)

else()
set (CG_USE_TEST_EXE OFF)
set (CG_EXPORT_LIBRARY OFF)
endif()

# use utf-16 little endian to read files
# NOTE: the data stored will be still stored with wide character. In some systems (such as Linux) wide characters are 
# stored as 4 bytes instead of 2.
set (USE_UTF16LE ON)

set (CG_LIB_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_lib)
set (CG_EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)

# Turn this to OFF if you want to print out more information about the resource build process.
set(CGRW_NO_VERBOSE OFF)

# add your resource by appending them to cg_resources list. The default "resource.cgures" should not be included in the list.
# example:
# list(APPEND CG_RESOURCES image_resource.cgures)

if (CG_USE_TEST_EXE MATCHES ON OR CG_EXPORT_LIBRARY MATCHES ON)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME})
    file (MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    
    set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/exe/${CMAKE_SYSTEM_NAME})
    file (MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
endif()

set_property(GLOBAL PROPERTY CG_RESOURCES ${CG_RESOURCES})
add_subdirectory(src/graphics)
add_subdirectory(src/log)
add_subdirectory(src/resource)
add_subdirectory(src/component)
add_subdirectory(src/game)
add_subdirectory(src/utils)
add_subdirectory(src/interface)
add_subdirectory(src/math)
add_subdirectory(src/input_handler)

if (CG_USE_TEST_EXE)
    list(APPEND CG_RESOURCES image_resource.cgures)
    list(APPEND CG_RESOURCES text_resource.cgures)
    list(APPEND CG_RESOURCES font_resource.cgures)
endif()

# opengl
find_package(OpenGL REQUIRED)
list(APPEND libs PUBLIC OpenGL::GL)

set(CG_INCLUDE_DIRECTORIES 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/dependencies/glad/include
    ${PROJECT_SOURCE_DIR}/dependencies/stb)

message("System: ${CMAKE_SYSTEM_NAME}")

#linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(glfw3 REQUIRED)
    list(APPEND libs "glfw")
    # todo: add freetype for linux
    list(APPEND libs "m")

    #todo: add freetype library

#windows
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if (CMAKE_SIZEOF_VOID_P STREQUAL 8)
        message("Building for: x64")
        list(APPEND CG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/64/include")
        list(APPEND CG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/dependencies/freetype/windows/64/include")
        if (MINGW)
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/64/lib-mingw-w64/libglfw3.a")
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/freetype/windows/64/mingw/freetype.a")
        elseif (MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/64/lib-vc2022/glfw3.lib")
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/freetype/windows/64/msvc/freetype.lib")
        endif()
    else()
    #todo: add freetype 32 bit support
        message("Building for: x86")
        list(APPEND CG_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/32/include")
        if (MINGW)
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/32/lib-mingw-w64/libglfw3.a")
        elseif (MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            list(APPEND libs "${PROJECT_SOURCE_DIR}/dependencies/glfw/windows/32/lib-vc2022/glfw3.lib")
        endif()
    endif()
endif()

if (USE_UTF16LE MATCHES ON)
    add_definitions(-DCG_USE_UTF16LE)
endif()

include_directories(
    ${CG_INCLUDE_DIRECTORIES}
)

if (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})

    add_subdirectory(dependencies/cos-graphics-resource-wrapper)

else()
    set(CGRW_PATH ${PROJECT_SOURCE_DIR}/dependencies/cos-graphics-resource-wrapper PARENT_SCOPE)
    set(CG_SOURCES ${CG_SOURCES} PARENT_SCOPE)
    set (CG_LIB_OUTPUT_NAME ${CG_LIB_OUTPUT_NAME} PARENT_SCOPE)
    set (USE_UTF16LE ${USE_UTF16LE} PARENT_SCOPE)
endif()

if (CG_EXPORT_LIBRARY)
    add_library(${CG_LIB_OUTPUT_NAME} STATIC
        ${CG_SOURCES}
    )

    target_link_libraries(${CG_LIB_OUTPUT_NAME} PUBLIC
        ${libs}
    )
else()
    add_library(${CG_LIB_OUTPUT_NAME}
        ${CG_SOURCES}
    )

    target_link_libraries(${CG_LIB_OUTPUT_NAME}
        ${libs}
    )
endif()

if (CG_USE_TEST_EXE)
    add_executable(${CG_EXE_OUTPUT_NAME}
        src/main.c
    )
    target_link_libraries(${CG_EXE_OUTPUT_NAME} PUBLIC
        ${CG_LIB_OUTPUT_NAME}
    )
endif()

add_subdirectory(test)