#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/component/camera.h"
#include "cos_graphics/utils.hpp"

CGSprite::CGSprite(const std::string& p_texture_rk) : CGVisualComponent()
{
    if (p_texture_rk == std::string(""))
        return;
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*& p_texture) : CGVisualComponent()
{
    texture = CGCopyVisualImage(p_texture);
}

CGSprite::CGSprite(CGVisualImage*&& p_texture) : texture(p_texture), CGVisualComponent()
{
    p_texture = nullptr;
}


CGSprite::CGSprite(const CGSprite& p_other) : CGVisualComponent(p_other)
{
    texture = CGCopyVisualImage(p_other.texture);
}

void CGSprite::SetTexture(CGVisualImage*& p_texture)
{
    texture = CGCopyVisualImage(p_texture);
}

void CGSprite::SetTexture(CGVisualImage*&& p_texture) noexcept
{
    texture = p_texture;
    p_texture = nullptr;
}

void CGSprite::SetTexture(const std::string& p_texture_rk)
{
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow());
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
    CGTransform global_transform = GetGlobalTransform();
    render_property->z = transform.depth;
    render_property->transform = global_transform.position;
    if (CGGame::GetInstance()->GetMainCamera() != nullptr)
        render_property->transform -= CGGame::GetInstance()->GetMainCamera()->GetTransform().position;
    render_property->rotation = global_transform.rotation;
    render_property->scale = global_transform.scale;
    
    CGDrawVisualImage(texture, render_property, CGGame::GetInstance()->GetGameWindow());
}