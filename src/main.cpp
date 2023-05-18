#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>

int main()
{
    CGGame::InitGame(640, 480, "Test");
    CGSprite* sprite = new CGSprite("./test2.png");
    CGSprite* sprite2 = new CGSprite("./test2.png");
    sprite2->SetPosition(CGConstructVector2(100.0f, 100.0f));

    CGGame::StartGame();
    return 0;
}