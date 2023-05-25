#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/resource.h"
#include <initializer_list>

class CGAnimationSprite : CGVisualComponent
{
protected:
    void Draw() override;

    /**
     * @brief How many seconds each frame is going to be displayed.
     */
    float frame_duration;

    /**
     * @brief The current frame that is being played.
     */
    unsigned int current_frame;

    /**
     * @brief The list of textures that is going to be played.
     */
    std::initializer_list<CGVisualImage*> texture_list;

public:

    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_texture_list The list of textures of the animation.
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, 
        const CGVector2& p_position = {0, 0});

    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_texture_list The list of textures of the animation.
     * @param p_fps The frame rate of the animation.
     * @param p_start_frame The frame that the animation is going to start.
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, 
        float p_fps, unsigned int p_start_frame, const CGVector2& p_position = {0, 0});

    virtual ~CGAnimationSprite();
};