#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>
#include <cos_graphics/log.h>

int main()
{
    CGGame::InitGame(640, 480, "Test");
    CGVisualImage* image = CGCreateVisualImage("./test1.png", 
        CGGame::GetInstance()->GetGameWindow());
    CGSprite* sprite = new CGSprite(image);
    CGSprite* sprite2 = new CGSprite(image);
    CGSprite* sprite3 = new CGSprite("./test2.png");
    sprite2->GetTransform().position = CGConstructVector2(100.0f, 100.0f);
    sprite2->SetDepth(-0.5f);
    sprite3->GetTransform().position = CGConstructVector2(-100.0f, -100.0f);
    CGGame::GetInstance()->SetWindowClearColor(CGConstructColor(0.0f, 0.0f, 0.0f, 1.0f));
    CGGame::StartGame();
    CGGame::ExitGame();
    return 0;
}