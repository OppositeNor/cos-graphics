#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>
#include <cos_graphics/component/animation_sprite.h>
#include <cos_graphics/log.h>

int main()
{
    CGGame::InitGame(1280, 1060, "Test", CG_FALSE, CG_FALSE);

    CGVisualImage* image = CGCreateVisualImage("test1", 
        CGGame::GetInstance()->GetGameWindow());
    CGSprite* sprite2 = new CGSprite(image);
    sprite2->GetTransform().position = CGConstructVector2(100.0f, 100.0f);
    CGSprite* sprite = new CGSprite(std::move(image));
    sprite->GetTransform().position = CGConstructVector2(-100.0f, 100.0f);

    CGAnimationSprite* animation_sprite = new CGAnimationSprite(5.0f);
    CGGame::GetInstance()->SetWindowClearColor(CGConstructColor(0.0f, 0.0f, 0.0f, 1.0f));
    
    CGGame::StartGame();
    return 0;
}