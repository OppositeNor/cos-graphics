project(cg_resource)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/resource.h
    ${PROJECT_SOURCE_DIR}/resource.c
    ${SOURCES}
    PARENT_SCOPE
)