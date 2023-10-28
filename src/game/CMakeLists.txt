project(cg_game)

set(CG_SOURCES

    ${CMAKE_SOURCE_DIR}/include/cos_graphics/game.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/game_factory.h

    ${PROJECT_SOURCE_DIR}/game.cpp
    ${PROJECT_SOURCE_DIR}/game_factory.cpp
    ${CG_SOURCES}
    PARENT_SCOPE
)