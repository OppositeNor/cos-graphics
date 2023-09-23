#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(const CGString& p_texture_rk) : CGVisualComponent()
{
    if (p_texture_rk == CGString(CGSTR("")))
        return;
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*& p_texture) : CGVisualComponent()
{
    texture = CGCopyVisualImage(p_texture);
}

CGSprite::~CGSprite()
{
    CGFree(texture);
}

void CGSprite::Draw(float p_delta)
{
    if (texture == nullptr)
        return;
    
    CGDrawVisualImage(texture, GetRenderProperty(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*&& p_texture) : texture(p_texture), CGVisualComponent()
{
    p_texture = nullptr;
}

CGSprite::CGSprite(const CGSprite& p_other) : CGVisualComponent(p_other)
{
    texture = CGCopyVisualImage(p_other.texture);
}

CGSprite::CGSprite(CGSprite&& p_other) noexcept : CGVisualComponent(std::move(p_other))
{
    texture = p_other.texture;
    p_other.texture = nullptr;
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

float CGSprite::GetWidth() const
{
    return transform.scale.x * texture->img_width;
}

float CGSprite::GetHeight() const
{
    return transform.scale.y * texture->img_height;
}