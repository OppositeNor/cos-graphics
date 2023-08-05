#include <cos_graphics/game.h>
#include <cos_graphics/component/sprite.h>
#include <cos_graphics/component/animation_sprite.h>
#include <cos_graphics/log.h>
#include <cos_graphics/component/camera.h>

CGSprite* sprite2 = nullptr;
void KeyCallback(CGWindow* window, int key, int action);

int main()
{
    CGGame::InitGame(1280, 720, "Test", CG_FALSE, CG_FALSE);
    CGSetKeyCallback(KeyCallback);

    CGVisualImage* image = CGCreateVisualImage("test1", 
        CGGame::GetInstance()->GetGameWindow());
    sprite2 = new CGSprite(image);
    sprite2->GetTransform().position = CGConstructVector2(100.0f, 100.0f);
    CGSprite* sprite = new CGSprite(std::move(image));
    sprite->GetTransform().position = CGConstructVector2(-100.0f, 100.0f);

    sprite->SetParent(sprite2);

    CGAnimationSprite* animation_sprite = new CGAnimationSprite(1.0f);
    animation_sprite->AddAnimation("test", std::vector<CGVisualImage*>{
        CGCreateVisualImage("test1", CGGame::GetInstance()->GetGameWindow()),
        CGCreateVisualImage("test2", CGGame::GetInstance()->GetGameWindow()),
        CGCreateVisualImage("test3", CGGame::GetInstance()->GetGameWindow()),
        CGCreateVisualImage("test4", CGGame::GetInstance()->GetGameWindow())});

    //animation_sprite->PlayFromStart("test");
    animation_sprite->SetAnimationFinishCallback([](CGAnimationSprite* p_animation_sprite)
    {
        static int i = 0;
        CG_PRINT("Animation finished %d", i++);
    });
    CGGame::GetInstance()->SetWindowClearColor(CGConstructColor(0.0f, 0.0f, 0.0f, 1.0f));
    
    CGGame::StartGame();
    return 0;
}

void KeyCallback(CGWindow* window, int key, int action)
{
    if (action == CG_PRESS)
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
        case CG_KEY_Q:
            if (sprite2)
                sprite2->GetTransform().scale.x += 0.1f;
            break;
        case CG_KEY_E:
            if (sprite2)
                sprite2->GetTransform().scale.x -= 0.1f;
            break;
        default:
            break;
        }
    }
}