#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/resource.h"
#include <initializer_list>
#include <map>

class CGAnimationSprite : CGVisualComponent
{
    CGRenderObjectProperty* render_property;
protected:

    void Draw(float p_delta) override;

    /**
     * @brief How many seconds each frame is going to be displayed.
     */
    float frame_duration;

    /**
     * @brief The current animation that is playing;
     */
    std::string current_animation;

    /**
     * @brief An internal clock of the animation.
     */
    float clock;

    /**
     * @brief The map the animations' textures.
     * @tparam std::string The name of the animation.
     * @tparam std::initializer_list<CGVisualImage*> The list of textures of the animation.
     */
    std::map<std::string, std::initializer_list<CGVisualImage*>> animation_map;

    /**
     * @brief Is the animation playing.
     */
    bool is_playing = false;

    /**
     * @brief The function that will be called after the animation is finished.
     * @param CGAnimationSprite* The animation that is finished.
     */
    void (*animation_finish_callback)(CGAnimationSprite*) = nullptr;

public:

    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_animation_map The map the animations' textures.
     * @param p_default_animation
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(const std::map<std::string, std::initializer_list<CGVisualImage*>>& p_animation_map, 
        std::string p_default_animation, const CGVector2& p_position = {0, 0});

    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_animation_map The map the animations' textures.
     * @param p_fps The frame rate of the animation.
     * @param p_start_frame The frame that the animation is going to start.
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(const std::map<std::string, std::initializer_list<CGVisualImage*>>& p_animation_map, 
        std::string p_default_animation, float p_fps, const CGVector2& p_position = {0, 0});

    /**
     * @brief Play the current animation.
     */
    void Play();
    /**
     * @brief Play the current animation from start.
     */
    void PlayFromStart();
    /**
     * @brief Play the animation from start.
     * 
     * @param p_animation_name The animation to be played.
     */
    void PlayFromStart(std::string p_animation_name);

    /**
     * @brief Set the animation finish callback function.
     * 
     * @param animation_finish_callback The function that will be called after the animation is finished.
     */
    void SetAnimationFinishCallback(void (*p_animation_finish_callback)(CGAnimationSprite*));

    virtual ~CGAnimationSprite();
};