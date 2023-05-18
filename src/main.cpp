#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>

int main()
{
    CGGame::InitGame(640, 480, "Test");
    CGSprite* sprite = new CGSprite("./test2.png");

    CGGame::StartGame();
    return 0;
}