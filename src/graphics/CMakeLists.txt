project(cg_graphics)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c

    ${CMAKE_SOURCE_DIR}/include/cos_graphics/graphics.h
    ${PROJECT_SOURCE_DIR}/graphics.c
    ${SOURCES}
    PARENT_SCOPE
)