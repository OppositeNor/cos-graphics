#pragma once
#include "cos_graphics/component/component.h"
#include "cos_graphics/graphics.h"

/**
 * @brief Visual component is the base class for all objects that are visible in the game world.
 * 
 */
class CGVisualComponent : public CGComponent
{
    CG_COMPONENT(CGVisualComponent)

    CGRenderObjectProperty* render_property;

    float matrix_buffer[4][4];
protected:
    /**
     * @brief Get the render property for objects to render.
     */
    CGRenderObjectProperty* GetRenderProperty() noexcept;
    
    const CGRenderObjectProperty* GetRenderProperty() const noexcept;

    /**
     * @brief Is the component visible in the game.
     */
    bool visible = true;

    /**
     * @brief Draw component on the window.
     */
    virtual void Draw(float p_delta) {};
public:
    /**
     * @brief Construct a new CGVisualComponent object.
     * 
     * @param p_position The initial position of the component.
     */
    CGVisualComponent();

    /**
     * @brief Copy a new CGVisualComponent object.
     * 
     * @param p_other The other CGVisualComponent object.
     */
    CGVisualComponent(const CGVisualComponent& p_other);

    /**
     * @brief Move constructor.
     * 
     * @param p_other The other CGVisualComponent object.
     */
    CGVisualComponent(CGVisualComponent&& p_other);

    virtual ~CGVisualComponent() override;

    /**
     * @brief Called every frame by the engine
     * 
     * @param p_delta_time The time difference between frames
     */
    virtual void Tick(double p_delta_time) override;

    /**
     * @brief Set if the component is visible in the game
     * 
     * @param p_visible The new value of visible
     */
    void SetVisible(bool p_visible) noexcept;

    /**
     * @brief Is the component visible in the game
     * 
     * @return true The component is visible in the game
     * @return false The component is not visible in the game
     */
    bool IsVisible() const noexcept;

    /**
     * @brief Set the color of the component.
     * 
     * @param p_color The color to be set to.
     */
    void SetColor(const CGColor& p_color) noexcept;
};