#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/graphics.h"
#include <string>
#include <memory>

class CGSprite : public CGVisualComponent
{
protected:
    /**
     * @brief The image of the sprite.
     */
    std::shared_ptr<CGVisualImage> image;

    CGRenderObjectProperty* render_property;

    void Draw() override;
public:
    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_img_path The path of the image for the sprite
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(std::string p_img_path = std::string(""), const CGVector2& p_position = {0, 0});

    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_image The image for the sprite.
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(CGVisualImage* p_image, const CGVector2& p_position = {0, 0});
    virtual ~CGSprite() override;

    /**
     * @brief Set the image of the sprite
     * 
     * @param p_image The image that the sprite is going to be set to.
     */
    void SetImage(CGVisualImage* p_image);

    /**
     * @brief Set the image of the sprite
     * 
     * @param p_img_path The path of the image file that the sprite is going to be set to.
     */
    void SetImage(std::string p_img_path);
};