cmake_minimum_required(VERSION 3.8)

project (CosGraphicsUnitTest C)

set (EXE_OUTPUT_NAME ${PROJECT_NAME}_${CMAKE_BUILD_TYPE}_executable)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib/${CMAKE_SYSTEM_NAME})

add_definitions(-DCG_NO_ERROR_NOTIFICATION)

set(TEST_SOURCES 
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/unit_test/unit_test.c  
    ${PROJECT_SOURCE_DIR}/unit_test/unit_test.h
    ${PROJECT_SOURCE_DIR}/test_main.c
    ${PROJECT_SOURCE_DIR}/test_utils/test_utils.c
    ${PROJECT_SOURCE_DIR}/test_utils/test_utils.h
    ${PROJECT_SOURCE_DIR}/test_resource/test_resource.c
    ${PROJECT_SOURCE_DIR}/test_resource/test_resource.h
    ${PROJECT_SOURCE_DIR}/test_graphics/test_graphics.c
    ${PROJECT_SOURCE_DIR}/test_graphics/test_graphics.h)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})