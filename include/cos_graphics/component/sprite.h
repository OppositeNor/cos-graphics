#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/graphics.h"
#include <string>

/**
 * @brief The sprite component. It can be used to render a component with a texture.
 * 
 * @bug The texture copy is still unusable.
 */
class CGSprite : public CGVisualComponent
{
    CG_COMPONENT_TYPE(CGSprite)
protected:
    /**
     * @brief The texture of the sprite.
     */
    CGVisualImage* texture;
    void Draw(float p_delta) override;
public:
    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_texture_rk The resource key of the texture for the sprite
     * @param p_position The position that the sprite is going to be on.
     */
    explicit CGSprite(const std::string& p_texture_rk = std::string(""));

    /**
     * @brief Copy constructor
     * 
     * @bug The texture copy is still unusable.
     * 
     * @param p_other The other CGSprite object.
     */
    CGSprite(const CGSprite& p_other);
    
    /**
     * @brief Construct a new CGSprite object
     * 
     * @bug The texture copy is still unusable.
     * 
     * @param p_texture The texture for the sprite. The texture is going to be copied.
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(CGVisualImage*& p_texture);

    /**
     * @brief Construct a new CGSprite object
     * 
     * @param p_texture The texture for the sprite. The texture is going to be moved.
     * @param p_position The position that the sprite is going to be on.
     */
    CGSprite(CGVisualImage*&& p_texture);

    virtual ~CGSprite() override;

    /**
     * @brief Set the texture of the sprite. The texture is going to be copied.
     * 
     * @bug The texture copy is still unusable.
     * 
     * @param p_texture The texture that the sprite is going to be set to.
     */
    void SetTexture(CGVisualImage*& p_texture);
    /**
     * @brief Set the texture of the sprite. The texture is going to be moved.
     * 
     * @param p_texture The texture that the sprite is going to be set to.
     */
    void SetTexture(CGVisualImage*&& p_texture) noexcept;

    /**
     * @brief Set the texture of the sprite
     * 
     * @param p_texture_rk The resource key of the texture file that the sprite is going to be set to.
     */
    void SetTexture(const std::string& p_texture_rk);
};