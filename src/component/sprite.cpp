#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(const std::string& p_img_path, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    if (p_img_path == std::string(""))
        return;
    image = std::shared_ptr<CGVisualImage>(CGCreateVisualImage(p_img_path.c_str(), 
        CGGame::GetInstance()->GetGameWindow()), CGDeleteVisualImage);
    
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

CGSprite::CGSprite(CGVisualImage* p_image, const CGVector2& p_position) : image(p_image), CGVisualComponent(p_position)
{
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

void CGSprite::SetImage(CGVisualImage* p_image)
{
    image = std::shared_ptr<CGVisualImage>(p_image);
}

void CGSprite::SetImage(const std::string& p_img_path)
{
    image = std::shared_ptr<CGVisualImage>(CGCreateVisualImage(p_img_path.c_str(), 
        CGGame::GetInstance()->GetGameWindow()), CGDeleteVisualImage);
}

CGSprite::~CGSprite()
{
    free(render_property);
}

void CGSprite::Draw()
{
    image->z = transform.depth;
    render_property->transform = transform.position;
    render_property->rotation = transform.rotation;
    render_property->scale = transform.scale;
    
    if (image == nullptr || !visual)
        return;
    CGDrawVisualImage(image.get(), render_property, CGGame::GetInstance()->GetGameWindow());
}