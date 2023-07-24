cmake_minimum_required(VERSION 3.8)

project(CosGraphics LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set (CMAKE_BUILD_TYPE Release)

message("Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DCG_DEBUG)
endif()

set(CG_TEST_MAIN_SOURCE src/main)

set(CG_CPP_TEST ON)



if(CG_CPP_TEST MATCHES ON)
    set(CG_TEST_MAIN_SOURCE ${CG_TEST_MAIN_SOURCE}.cpp)
else()
    set(CG_TEST_MAIN_SOURCE ${CG_TEST_MAIN_SOURCE}.c)
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# set this to OFF if you only want to export the library
set (USE_TEST_EXE ON)

# set this to OFF if you don't want to export the library
set (USE_SHARED_LIB ON)

set (LIB_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_lib)
set (EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)

# Turn this to OFF if you want to print out more information about the resource build process.
set(CGRW_NO_VERBOSE OFF)

# add your resource by appending them to cg_resources list. The default "resource.cgures" should not be included in the list.
list(APPEND CG_RESOURCES image_resource.cgures)


set_property(GLOBAL PROPERTY CG_RESOURCES ${CG_RESOURCES})
add_subdirectory(src/graphics)
add_subdirectory(src/log)
add_subdirectory(src/resource)
add_subdirectory(src/component)
add_subdirectory(src/game)

add_subdirectory(dependencies/cos-graphics-resource-wrapper)


# opengl
find_package(OpenGL REQUIRED)
list(APPEND libs PUBLIC OpenGL::GL)

set(CG_INCLUDE_DIRECTORIES 
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include
    ${CMAKE_SOURCE_DIR}/dependencies/stb)

message("System: ${CMAKE_SYSTEM_NAME}")

#linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # GLFW
    find_package(glfw3 REQUIRED)
    list(APPEND libs PUBLIC glfw)

    list(APPEND libs PUBLIC m)
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
        ${CG_TEST_MAIN_SOURCE}
    )
    target_link_libraries(${EXE_OUTPUT_NAME}
        ${LIB_OUTPUT_NAME}
    )
endif()
