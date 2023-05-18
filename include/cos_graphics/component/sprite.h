#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/graphics.h"
#include <string>

class CGSprite : public CGVisualComponent
{
protected:
    /**
     * @brief The image of the sprite.
     */
    CGVisualImage* image = nullptr;

    virtual void Draw() override;
public:
    CGSprite(std::string p_img_path = std::string(""));
    virtual ~CGSprite() override;
};