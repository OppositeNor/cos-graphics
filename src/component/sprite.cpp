#include "cos_graphics/component/sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGSprite::CGSprite(const CGString& p_texture_rk) : CGTextureContainer()
{
    texture = nullptr;
    if (p_texture_rk == CGString(CGSTR("")))
        return;
    texture = CGCreateVisualImage(p_texture_rk.c_str(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*& p_texture) : CGTextureContainer()
{
    texture = CGCopyVisualImage(p_texture);
}

CGSprite::~CGSprite()
{
    
}

void CGSprite::Draw(float p_delta)
{
    if (texture == nullptr)
        return;
    
    CGDrawVisualImage(texture, GetRenderProperty(), CGGame::GetInstance()->GetGameWindow());
}

CGSprite::CGSprite(CGVisualImage*&& p_texture) : CGTextureContainer()
{
    texture = p_texture;
    p_texture = nullptr;
}

CGSprite::CGSprite(const CGSprite& p_other) : CGTextureContainer(p_other)
{

}

CGSprite::CGSprite(CGSprite&& p_other) noexcept : CGTextureContainer(std::move(p_other))
{

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