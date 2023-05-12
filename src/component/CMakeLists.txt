project(cg_component)

set (SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/component.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/visual_component.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/animation_sprite.h

    ${PROJECT_SOURCE_DIR}/component.cpp
    ${PROJECT_SOURCE_DIR}/visual_component.cpp
    ${PROJECT_SOURCE_DIR}/animation_sprite.cpp
    ${SOURCES}
    PARENT_SCOPE
)

