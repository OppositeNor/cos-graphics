project(cg_graphics)

# opengl
find_package(OpenGL REQUIRED)
LIST(APPEND libs OpenGL::GL)

# GLFW
find_package(glfw3 REQUIRED)
LIST(APPEND libs glfw)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include)

add_library(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/graphics.h
    graphics.c
)
target_link_libraries(${PROJECT_NAME}
    ${libs}
    m
)