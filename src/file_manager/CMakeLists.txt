project(cg_file_manager)

set(CG_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/file_manager.h
    ${PROJECT_SOURCE_DIR}/file_manager.c
    ${SOURCES}
    PARENT_SCOPE
)