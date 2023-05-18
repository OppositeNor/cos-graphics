#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(std::string p_img_path) : CGVisualComponent()
{
    if (p_img_path == std::string(""))
        return;
    image = CGCreateVisualImage(
        p_img_path.c_str(), 
        CGCreateRenderObjectProperty(
            CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f), 
            CGConstructVector2(0.0f, 0.0f), 
            CGConstructVector2(1.0f, 1.0f), 
            0.0f), 
        CGGame::GetInstance()->GetGameWindow());
}

CGSprite::~CGSprite()
{
    free(image->property);
    CGDeleteVisualImage(image);
}

void CGSprite::Draw()
{
    if (image == nullptr || !visual)
        return;
    CGDrawVisualImage(image, CGGame::GetInstance()->GetGameWindow());
}