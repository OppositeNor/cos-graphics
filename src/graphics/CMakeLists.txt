# Graphics

set(CG_SOURCES
    ${PROJECT_SOURCE_DIR}/dependencies/glad/src/glad.c

    ${PROJECT_SOURCE_DIR}/include/cos_graphics/graphics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c
    ${CG_SOURCES}
    PARENT_SCOPE
)