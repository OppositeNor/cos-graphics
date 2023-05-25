#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(const std::string& p_texture_path, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    if (p_texture_path == std::string(""))
        return;
    texture = std::shared_ptr<CGVisualImage>(CGCreateVisualImage(p_texture_path.c_str(), 
        CGGame::GetInstance()->GetGameWindow()), CGDeleteVisualImage);
    
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

CGSprite::CGSprite(CGVisualImage* p_texture, const CGVector2& p_position) : texture(p_texture), CGVisualComponent(p_position)
{
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

void CGSprite::SetImage(CGVisualImage* p_texture)
{
    texture = std::shared_ptr<CGVisualImage>(p_texture);
}

void CGSprite::SetImage(const std::string& p_texture_path)
{
    texture = std::shared_ptr<CGVisualImage>(CGCreateVisualImage(p_texture_path.c_str(), 
        CGGame::GetInstance()->GetGameWindow()), CGDeleteVisualImage);
}

CGSprite::~CGSprite()
{
    free(render_property);
}

void CGSprite::Draw(float p_delta)
{
    if (texture == nullptr)
        return;
    
    texture->z = transform.depth;
    render_property->transform = transform.position;
    render_property->rotation = transform.rotation;
    render_property->scale = transform.scale;
    
    CGDrawVisualImage(texture.get(), render_property, CGGame::GetInstance()->GetGameWindow());
}