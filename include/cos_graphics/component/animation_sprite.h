#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/resource.h"
#include <initializer_list>

class CGAnimationSprite : CGVisualComponent
{
protected:
    void Draw() override;

    /**
     * @brief The frame count that is played in one second.
     */
    float fps;

    /**
     * @brief The current frame that is being played.
     */
    unsigned int current_frame;

    /**
     * @brief The list of textures that is going to be played.
     */
    std::initializer_list<CGVisualImage*> texture_list;

public:

    CGAnimationSprite(const std::initializer_list<CGVisualImage*>& p_texture_list, const CGVector2& p_position = {0, 0});

    virtual ~CGAnimationSprite();

    
};