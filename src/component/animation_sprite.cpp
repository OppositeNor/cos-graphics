#include "cos_graphics/component/animation_sprite.h"
#include "cos_graphics/game.h"
#include <iostream>

CGAnimationSprite::CGAnimationSprite(const std::map<std::string, std::initializer_list<CGVisualImage*>>& p_animation_map, 
        std::string p_default_animation, const CGVector2& p_position)
    : animation_map(p_animation_map), current_animation(p_default_animation), frame_duration(0.2f), CGVisualComponent(p_position)
{
    render_property = CGCreateRenderObjectProperty(CGConstructColor(1.0f, 1.0f, 1.0f,1.0f), 
        transform.position, transform.scale, transform.rotation);
}


        
CGAnimationSprite::CGAnimationSprite(const std::map<std::string, std::initializer_list<CGVisualImage*>>& p_animation_map, 
        std::string p_default_animation, float p_fps, const CGVector2& p_position)
    : animation_map(p_animation_map), current_animation(p_default_animation), frame_duration(GetReciprocal(p_fps)), 
        CGVisualComponent(p_position)
{
    render_property = CGCreateRenderObjectProperty(CGConstructColor(1.0f, 1.0f, 1.0f,1.0f), 
        transform.position, transform.scale, transform.rotation);
}

CGAnimationSprite::~CGAnimationSprite()
{
    free(render_property);
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