#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(std::string p_img_path, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    if (p_img_path == std::string(""))
        return;
    image = std::shared_ptr<CGVisualImage>(CGCreateVisualImage(
        p_img_path.c_str(), 
        CGCreateRenderObjectProperty(
            CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f), 
            CGConstructVector2(0.0f, 0.0f), 
            CGConstructVector2(1.0f, 1.0f), 
            0.0f), 
        CGGame::GetInstance()->GetGameWindow()), CGDeleteVisualImage);
}

CGSprite::CGSprite(CGVisualImage* p_image, const CGVector2& p_position) : image(p_image), CGVisualComponent(p_position)
{}

CGSprite::~CGSprite()
{
    free(image->property);
}

void CGSprite::Draw()
{
    image->property->transform = transform.position;
    image->property->rotation = transform.rotation;
    image->property->scale = transform.scale;
    image->z = transform.depth;
    has_transform_set = false;
    
    if (image == nullptr || !visual)
        return;
    CGDrawVisualImage(image.get(), CGGame::GetInstance()->GetGameWindow());
}