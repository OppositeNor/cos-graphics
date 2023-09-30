project(cg_math)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/math/matrix.hpp
    ${PROJECT_SOURCE_DIR}/matrix.cpp
    ${SOURCES}
    PARENT_SCOPE
)