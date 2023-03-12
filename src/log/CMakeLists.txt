project(cg_log)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/log.h
    ${PROJECT_SOURCE_DIR}/log.c
    ${SOURCES}
    PARENT_SCOPE
)