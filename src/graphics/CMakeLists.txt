# Graphics

set(SOURCES
    ${PROJECT_SOURCE_DIR}/dependencies/glad/src/glad.c

    ${PROJECT_SOURCE_DIR}/include/cos_graphics/graphics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c
    ${SOURCES}
    PARENT_SCOPE
)