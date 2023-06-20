#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/graphics.h"
#include <string>

class CGSprite : public CGVisualComponent
{

protected:
    /**
     * @brief The texture of the sprite.
     */
    CGVisualImage* texture;

    void Draw(float p_delta) override;

    /**
     * @brief Whether the texture is shared or not.
     */
    bool is_texture_shared;
public:
    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_texture_rk The resource key of the texture for the sprite
     * @param p_position The position that the sprite is going to be on.
     */
    explicit CGSprite(const std::string& p_texture_rk = std::string(""), const CGVector2& p_position = {0, 0});

    
    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_texture The texture for the sprite.
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(CGVisualImage*& p_texture, const CGVector2& p_position = {0, 0});

    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_texture The texture for the sprite.
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(CGVisualImage*&& p_texture, const CGVector2& p_position = {0, 0});
    virtual ~CGSprite() override;

    /**
     * @brief Set the texture of the sprite
     * 
     * @param p_texture The texture that the sprite is going to be set to.
     */
    void SetTexture(CGVisualImage*& p_texture);
    /**
     * @brief Set the texture of the sprite
     * 
     * @param p_texture The texture that the sprite is going to be set to.
     */
    void SetTexture(CGVisualImage*&& p_texture);

    /**
     * @brief Set the texture of the sprite
     * 
     * @param p_texture_rk The resource key of the texture file that the sprite is going to be set to.
     */
    void SetTexture(const std::string& p_texture_rk);
};