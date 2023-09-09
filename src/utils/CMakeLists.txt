project(cg_utils)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/utils.h
    ${PROJECT_SOURCE_DIR}/utils.c
    ${SOURCES}
    PARENT_SCOPE
)