#include "cos_graphics/component/animation_sprite.h"
#include "cos_graphics/game.h"
#include <iostream>


CGAnimationSprite::CGAnimationSprite(std::map<std::string, std::initializer_list<CGVisualImage*>>& p_animation_map, 
        std::string p_default_animation, float p_fps, const CGVector2& p_position)
    : animation_map(p_animation_map), current_animation(p_default_animation), frame_duration(GetReciprocal(p_fps)), 
        CGVisualComponent(p_position)
{
    is_texture_shared = true;
}

CGAnimationSprite::CGAnimationSprite(std::map<std::string, std::initializer_list<CGVisualImage*>>&& p_animation_map, 
        std::string p_default_animation, float p_fps, const CGVector2& p_position)
    : animation_map(std::move(p_animation_map)), current_animation(p_default_animation), frame_duration(GetReciprocal(p_fps)), 
        CGVisualComponent(p_position)
{
    is_texture_shared = false;
}

CGAnimationSprite::~CGAnimationSprite()
{
    CGFreeResource(render_property);
    if (!is_texture_shared)
    {
        // Free all the textures
        for (auto& animation : animation_map)
        {
            for (auto& frame : animation.second)
            {
                CGFreeResource(frame);
            }
        }
    }
}

void CGAnimationSprite::Play()
{
    is_playing = true;
}


void CGAnimationSprite::PlayFromStart()
{
    is_playing = true;
    clock = 0.0f;
}

void CGAnimationSprite::PlayFromStart(std::string p_animation_name)
{
    is_playing = true;
    current_animation = p_animation_name;
    clock = 0.0f;
}

void CGAnimationSprite::SetAnimationFinishCallback(void (*p_animation_finish_callback)(CGAnimationSprite*))
{
    animation_finish_callback = p_animation_finish_callback;
}

void CGAnimationSprite::Draw(float p_delta)
{
    auto animation_played = animation_map[current_animation];
    if (animation_played.size() == 0)
        return;
    if (is_playing)
        clock += p_delta;
    int current_frame = (int)(frame_duration / clock);
    if (current_frame >= animation_played.size())
    {
        if (animation_finish_callback != nullptr)
            animation_finish_callback(this);
        current_frame = 0;
    }
    auto frame_displayed = *(animation_played.begin() + current_frame);
    render_property->z = transform.depth;
    render_property->transform = transform.position;
    render_property->rotation = transform.rotation;
    render_property->scale = transform.scale;

    CGDrawVisualImage(frame_displayed, render_property, CGGame::GetInstance()->GetGameWindow());
}