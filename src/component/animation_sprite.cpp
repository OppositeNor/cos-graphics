#include "cos_graphics/component/animation_sprite.h"
#include <iostream>

CGAnimationSprite::CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, const CGVector2& p_position)
    : texture_list(p_texture_list), frame_duration(0.2f), current_frame(0), CGVisualComponent(p_position)
{
    
}


        
CGAnimationSprite::CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, 
    float p_fps, unsigned int p_start_frame, const CGVector2& p_position)
    : texture_list(p_texture_list), frame_duration(GetReciprocal(p_fps)), current_frame(p_start_frame), CGVisualComponent(p_position)
{
    
}

CGAnimationSprite::~CGAnimationSprite()
{
    
}