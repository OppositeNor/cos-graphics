#include "cos_graphics/component/animation_sprite.h"
#include <iostream>

CGAnimationSprite::CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, const CGVector2& p_position)
    : texture_list(p_texture_list), CGVisualComponent(p_position)
{
    
}

CGAnimationSprite::~CGAnimationSprite()
{
    
}