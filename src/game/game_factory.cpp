#include "cos_graphics/game_factory.h"
#include "cos_graphics/game.h"
#include "cos_graphics/component/component.h"

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

CGComponent* CGGameFactory::CreateRootComponent() const
{
    return new CGComponent();
}