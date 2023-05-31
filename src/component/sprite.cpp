#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(const std::string& p_texture_path, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    if (p_texture_path == std::string(""))
        return;
    texture = CGCreateVisualImage(p_texture_path.c_str(), CGGame::GetInstance()->GetGameWindow());
    is_texture_shared = false;
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

CGSprite::CGSprite(CGVisualImage*& p_texture, const CGVector2& p_position) : 
    texture(p_texture), CGVisualComponent(p_position)
{
    is_texture_shared = true;
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

CGSprite::CGSprite(CGVisualImage*&& p_texture, const CGVector2& p_position) : 
    texture(p_texture), CGVisualComponent(p_position)
{
    is_texture_shared = false;
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}

void CGSprite::SetTexture(CGVisualImage*& p_texture)
{
    texture = p_texture;
    if (!is_texture_shared)
        CGFreeResource(texture);
    is_texture_shared = true;
}

void CGSprite::SetTexture(CGVisualImage*&& p_texture)
{
    texture = p_texture;
    if (!is_texture_shared)
        CGFreeResource(texture);
    is_texture_shared = false;
}

void CGSprite::SetTexture(const std::string& p_texture_path)
{
    texture = CGCreateVisualImage(p_texture_path.c_str(), CGGame::GetInstance()->GetGameWindow());
    if (!is_texture_shared)
        CGFreeResource(texture);
    is_texture_shared = false;
}

CGSprite::~CGSprite()
{
    CGFreeResource(render_property);
    if(!is_texture_shared)
        CGFreeResource(texture);
}

void CGSprite::Draw(float p_delta)
{
    if (texture == nullptr)
        return;
    
    render_property->z = transform.depth;
    render_property->transform = transform.position;
    render_property->rotation = transform.rotation;
    render_property->scale = transform.scale;
    
    CGDrawVisualImage(texture, render_property, CGGame::GetInstance()->GetGameWindow());
}