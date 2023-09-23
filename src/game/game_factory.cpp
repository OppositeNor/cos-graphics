#include "cos_graphics/game_factory.h"
#include "cos_graphics/game.h"

CGGameFactory::CGGameFactory()
{
}

CGGameFactory::~CGGameFactory()
{
}

CGGame* CGGameFactory::CreateGame() const
{
    return new CGGame();
}