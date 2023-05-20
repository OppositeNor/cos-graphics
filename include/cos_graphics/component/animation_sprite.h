#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/resource.h"
#include <vector>
class CGAnimationSprite : CGVisualComponent
{
protected:
    void Draw() override;

    float fps;

public:

    CGAnimationSprite(const CGVector2& p_position);
    virtual ~CGAnimationSprite();
};

struct CGFrame
{
    CGImage* image;
};