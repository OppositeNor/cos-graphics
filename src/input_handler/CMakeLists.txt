project(cg_input_handler)

set(CG_SOURCES

    ${CMAKE_SOURCE_DIR}/include/cos_graphics/input_handler.h
    ${PROJECT_SOURCE_DIR}/input_handler.cpp
    ${CG_SOURCES}
    PARENT_SCOPE
)