#pragma once

#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/graphics.h"

class CGTextureContainer : public CGVisualComponent
{
    CG_COMPONENT(CGTextureContainer)
protected:
    CGVisualImage* texture;
public:
    /**
     * @brief Construct a new CGTextureContainer object
     */
    CGTextureContainer() : texture(nullptr), CGVisualComponent() {}
    /**
     * @brief Copy constructor
     */
    CGTextureContainer(const CGTextureContainer& p_other);

    /**
     * @brief Move constructor
     */
    CGTextureContainer(CGTextureContainer&& p_other) noexcept;

    /**
     * @brief Destroy the CGTextureContainer object
     */
    virtual ~CGTextureContainer() override;


    /**
     * @brief Get the width of the visual component.
     * 
     * @return float The width of the visual component.
     */
    virtual float GetBoarderWidth() const noexcept override;
    /**
     * @brief Get the height of the visual component
     * 
     * @return float The height of the visual component
     */
    virtual float GetBoarderHeight() const noexcept override;

    /**
     * @brief Get the y coordinate value of the top of the boarder.
     * 
     * @return float The y coordinate value of the top of the boarder.
     */
    virtual float GetBoarderTopY() const noexcept override;
    /**
     * @brief Get the y coordinate value of the bottom of the boarder.
     * 
     * @return float The y coordinate value of the bottom of the boarder.
     */
    virtual float GetBoarderBottomY() const noexcept override;

    /**
     * @brief Get the x coordinate value of the left of the boarder.
     * 
     * @return float The x coordinate value of the left of the boarder.
     */
    virtual float GetBoarderLeftX() const noexcept override;

    /**
     * @brief Get the x coordinate value of the right of the boarder.
     * 
     * @return float The x coordinate value of the right of the boarder.
     */
    virtual float GetBoarderRightX() const noexcept override;
};