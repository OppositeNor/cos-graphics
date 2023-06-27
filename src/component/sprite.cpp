#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/component/camera.h"

CGSprite::CGSprite(const std::string& p_texture_rk, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    if (p_texture_rk == std::string(""))
        return;
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*& p_texture, const CGVector2& p_position) : CGVisualComponent(p_position)
{
    texture = CGCopyVisualImage(p_texture);
}

CGSprite::CGSprite(CGVisualImage*&& p_texture, const CGVector2& p_position) : 
    texture(p_texture), CGVisualComponent(p_position)
{
    p_texture = nullptr;
    render_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        transform.position,
        transform.scale,
        transform.rotation
    );
}


CGSprite::CGSprite(const CGSprite& p_other) : CGVisualComponent(p_other)
{
    texture = CGCopyVisualImage(p_other.texture);

    render_property = CGCreateRenderObjectProperty(
        p_other.render_property->color,
        p_other.transform.position,
        p_other.transform.scale,
        p_other.transform.rotation
    );
}

void CGSprite::SetTexture(CGVisualImage*& p_texture)
{
    texture = CGCopyVisualImage(p_texture);
}

void CGSprite::SetTexture(CGVisualImage*&& p_texture)
{
    texture = p_texture;
    p_texture = nullptr;
}

void CGSprite::SetTexture(const std::string& p_texture_rk)
{
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow()), CGFreeResource;
}

CGSprite::~CGSprite()
{
    CGFreeResource(texture);
    CGFreeResource(render_property);
}

void CGSprite::Draw(float p_delta)
{
    if (texture == nullptr)
        return;
    
    render_property->z = transform.depth;
    render_property->transform = transform.position;
    if (CGGame::GetInstance()->GetMainCamera() != nullptr)
        render_property->transform -= CGGame::GetInstance()->GetMainCamera()->GetTransform().position;
    render_property->rotation = transform.rotation;
    render_property->scale = transform.scale;
    
    CGDrawVisualImage(texture, render_property, CGGame::GetInstance()->GetGameWindow());
}