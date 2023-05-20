#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>

int main()
{
    CGGame::InitGame(640, 480, "Test");
    CGVisualImage* image = CGCreateVisualImage("./test2.png", 
        CGCreateRenderObjectProperty(
            CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f), 
            CGConstructVector2(0.0f, 0.0f), 
            CGConstructVector2(1.0f, 1.0f), 
            0.0f), CGGame::GetInstance()->GetGameWindow());
    CGSprite* sprite = new CGSprite(image);
    CGSprite* sprite2 = new CGSprite(image);
    sprite2->GetTransform().position = CGConstructVector2(100.0f, 100.0f);
    sprite2->SetDepth(-0.5f);
    CGGame::GetInstance()->SetWindowClearColor(CGConstructColor(0.0f, 0.0f, 0.0f, 1.0f));
    CGGame::StartGame();
    return 0;
}