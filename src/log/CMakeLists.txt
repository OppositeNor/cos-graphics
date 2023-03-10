project(cg_log)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_library(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/log.h
    log.c
)