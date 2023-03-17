project(cg_log)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/log.h
    ${PROJECT_SOURCE_DIR}/log.c
    ${SOURCES}
    PARENT_SCOPE
)