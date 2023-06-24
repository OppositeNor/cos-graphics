cmake_minimum_required(VERSION 3.8)

project(cg_resource_wrapper)
set(CMAKE_BUILD_TYPE "Release")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CGRW_EXECUTABLE_NAME "cg_resource_wrapper.exe" PARENT_SCOPE)
else()
    set(CGRW_EXECUTABLE_NAME "cg_resource_wrapper" PARENT_SCOPE)
endif()

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME})
file (MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
include_directories(
    ${PROJECT_SOURCE_DIR}/include/
)

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

if (EXISTS ${EXECUTABLE_OUTPUT_PATH}/${CGRW_EXECUTABLE_NAME})
    execute_process(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ./dependencies/cos-graphics-resource-wrapper/bin/${CMAKE_SYSTEM_NAME}/${PROJECT_NAME} ./bin/exe/${CMAKE_SYSTEM_NAME}
    )
endif()

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/include/cg_resource_wrapper/resource_wrapper.h
    ${PROJECT_SOURCE_DIR}/src/resource_wrapper.c
    ${PROJECT_SOURCE_DIR}/src/main.c

    # log
    ${PROJECT_SOURCE_DIR}/include/cg_resource_wrapper/log.h
    ${PROJECT_SOURCE_DIR}/src/log.c
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ./dependencies/cos-graphics-resource-wrapper/bin/${CMAKE_SYSTEM_NAME}/${PROJECT_NAME} ./bin/exe/${CMAKE_SYSTEM_NAME} ${CG_RESOURCES}
)