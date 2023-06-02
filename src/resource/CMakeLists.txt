project(cg_resource)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/resource.h
    ${PROJECT_SOURCE_DIR}/resource.c
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/resource_ext.h
    ${PROJECT_SOURCE_DIR}/resource_cpp_ext.cpp
    ${SOURCES}
    PARENT_SCOPE
)