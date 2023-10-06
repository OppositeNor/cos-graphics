#if 0
#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>
#include <cos_graphics/component/animation_sprite.h>
#include <cos_graphics/log.h>
#include <cos_graphics/component/camera.h>
#include <cos_graphics/component/text.h>

CGSprite* sprite2 = nullptr;
CGSprite* sprite = nullptr;
void KeyCallback(CGWindow* window, int key, int action);

int main()
{
    CGGame::InitGame(1280, 720, CGSTR("Test"), CG_FALSE, CG_FALSE);
    CGSetKeyCallback(KeyCallback);

    CGVisualImage* image = CGCreateVisualImage(CGSTR("test1"), 
        CGGame::GetInstance()->GetGameWindow());
    sprite2 = new CGSprite(image);
    sprite2->GetTransform().position = CGConstructVector2(100.0f, 100.0f);
    auto sprite3 = new CGSprite(image);
    sprite3->GetTransform().position = CGConstructVector2(-100.0f, -100.0f);
    sprite = new CGSprite(std::move(image));
    sprite->GetTransform().position = CGConstructVector2(-100.0f, 100.0f);

    sprite->SetParent(sprite2);
    sprite3->SetParent(sprite);
    CGAnimationSprite* animation_sprite = new CGAnimationSprite(5.0f);
    animation_sprite->AddAnimation(CGSTR("test"), std::vector<CGVisualImage*>{
        CGCreateVisualImage(CGSTR("test1"), CGGame::GetInstance()->GetGameWindow()),
        CGCreateVisualImage(CGSTR("test2"), CGGame::GetInstance()->GetGameWindow()),
        CGCreateVisualImage(CGSTR("test3"), CGGame::GetInstance()->GetGameWindow())});
    
    animation_sprite->GetTransform().scale = CGConstructVector2(5.0f, 5.0f);
    animation_sprite->PlayFromStart(CGSTR("test"));
    
    auto text = new CGText(CGSTR("test_text"), CGConstructTextProperty(120, 120, 30, 10));
    text->GetTransform().scale = CGConstructVector2(0.5f, 0.5f);

    CGGame::GetInstance()->SetWindowClearColor(CGConstructColor(0.2f, 0.2f, 0.0f, 1.0f));
    
    CGGame::StartGame();
    delete sprite2;
    delete sprite3;
    delete sprite;
    CGGame::ExitGame();
    return 0;
}

void KeyCallback(CGWindow* window, int key, int action)
{
    if (action == CG_REPEAT || action == CG_PRESS)
    {
        switch(key)
        {
        case CG_KEY_ESCAPE:
            CGGame::GetInstance()->ExitGame();
            break;
        case CG_KEY_A:
            if (sprite2)
                sprite2->GetTransform().position.x -= 10.0f;
            break;
        case CG_KEY_D:
            if (sprite2)
                sprite2->GetTransform().position.x += 10.0f;
            break;
        case CG_KEY_W:
            if (sprite2)
                sprite2->GetTransform().position.y += 10.0f;
            break;
        case CG_KEY_S:
            if (sprite2)
                sprite2->GetTransform().position.y -= 10.0f;
            break;
        case CG_KEY_R:
            if (sprite2)
                sprite2->GetTransform().rotation += 0.1f;
            break;
        case CG_KEY_T:
            if (sprite2)
                sprite2->GetTransform().rotation -= 0.1f;
            break;
        case CG_KEY_Q:
            if (sprite2)
                sprite2->GetTransform().scale.x += 0.05f;
            break;
        case CG_KEY_E:
            if (sprite2)
                sprite2->GetTransform().scale.x -= 0.05f;
            break;
        case CG_KEY_UP:
            if (sprite)
                sprite->GetTransform().position.y += 10.0f;
            break;
        case CG_KEY_DOWN:
            if (sprite)
                sprite->GetTransform().position.y -= 10.0f;
            break;
        case CG_KEY_LEFT:
            if (sprite)
                sprite->GetTransform().position.x -= 10.0f;
            break;
        case CG_KEY_RIGHT:
            if (sprite)
                sprite->GetTransform().position.x += 10.0f;
            break;
        default:
            break;
        }
    }
}
#endif
#if 1
#include "cos_graphics/game.h"
#include "cos_graphics/component/sprite.h"
#include "cos_graphics/component/text.h"
int main()
{
    CGGame::InitGame(1280, 720, CGSTR(""));
    auto sprite = new CGSprite(CGCreateVisualImage(CGSTR("test1"), CGGame::GetInstance()->GetGameWindow()));
    auto sprite2 = new CGSprite(CGCreateVisualImage(CGSTR("test1"), CGGame::GetInstance()->GetGameWindow()));
    sprite->AddChild(sprite2);
    sprite->GetTransform().scale = CGConstructVector2(2.0f, 2.0f);
    sprite2->GetTransform().position = CGConstructVector2(100.0f, -100.0f);
    sprite->AllignLeft();
    auto sprite3 = new CGSprite(CGCreateVisualImage(CGSTR("test1"), CGGame::GetInstance()->GetGameWindow()));
    sprite3->GetTransform().position = CGConstructVector2(0.0f, -100.0f);
    sprite3->AllignLeftToRight(sprite2);
    sprite->SetVisible(false);
    CGGame::GetInstance()->QueueFree(sprite3);
    //auto text = new CGText(CGSTR("test_text"), CGConstructTextProperty(60, 60, 20, 5));
    CGGame::GetInstance()->StartGame();
    delete sprite;
    delete sprite2;
    delete sprite3;
    CGGame::GetInstance()->ExitGame();
    return 0;
}
#endif