project(cg_component)

set (SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/animation_sprite.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/camera.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/component.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/texture_container.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/sprite.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/text.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/component/visual_component.h

    ${PROJECT_SOURCE_DIR}/animation_sprite.cpp
    ${PROJECT_SOURCE_DIR}/camera.cpp
    ${PROJECT_SOURCE_DIR}/component.cpp
    ${PROJECT_SOURCE_DIR}/texture_container.cpp
    ${PROJECT_SOURCE_DIR}/sprite.cpp
    ${PROJECT_SOURCE_DIR}/text.cpp
    ${PROJECT_SOURCE_DIR}/visual_component.cpp
    ${SOURCES}
    PARENT_SCOPE
)

