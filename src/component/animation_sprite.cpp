#include "cos_graphics/component/animation_sprite.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/utils.hpp"


CGAnimationSprite::CGAnimationSprite(CGAnimationMap& p_animation_map, 
        std::string p_default_animation, float p_fps)
    : CGAnimationSprite(p_fps)
{
    animation_map = CGAnimationMap(p_animation_map);
    current_animation = p_default_animation;
}

CGAnimationSprite::CGAnimationSprite(CGAnimationMap&& p_animation_map, 
        std::string p_default_animation, float p_fps)
    : CGAnimationSprite(p_fps)
{
    animation_map = std::move(p_animation_map);
    current_animation = p_default_animation;
}

void CGAnimationSprite::CGAddAnimation(const std::string& p_animation_name, const std::vector<CGVisualImage*>& p_animation)
{
    animation_map.insert(std::make_pair(p_animation_name, p_animation));
}

void CGAnimationSprite::CGAddAnimation(const CGAnimationPair& p_animation_pair)
{
    animation_map.insert(p_animation_pair);
}

CGAnimationSprite::CGAnimationSprite(float p_fps)
    : frame_duration(CGUtils::CGGetReciprocal(p_fps)), CGVisualComponent()
{
    
}

CGAnimationSprite::~CGAnimationSprite()
{
    CGFreeResource(render_property);
    // Free all the textures
    for (auto&& animation : animation_map)
    {
        for (auto&& frame : animation.second)
        {
            CGFreeResource(frame);
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
    if (animation_map.size() == 0)
        return;
    if (animation_map.find(current_animation) == animation_map.end())
    {
        CG_WARNING("Animation %s not found.", current_animation.c_str());
        return;
    }
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