project(cg_game)

set(SOURCES

    ${CMAKE_SOURCE_DIR}/include/cos_graphics/game.h
    ${CMAKE_SOURCE_DIR}/include/cos_graphics/game_factory.h

    ${PROJECT_SOURCE_DIR}/game.cpp
    ${PROJECT_SOURCE_DIR}/game_factory.cpp
    ${SOURCES}
    PARENT_SCOPE
)