#pragma once
#include "cos_graphics/component/component.h"
#include "cos_graphics/graphics.h"

/**
 * @brief Visual component is the base class for all objects that are visual in the game world.
 * 
 */
class CGVisualComponent : public CGComponent
{
    CG_COMPONENT(CGVisualComponent)

    CGRenderObjectProperty* render_property;
protected:
    /**
     * @brief Get the render property for objects to render.
     */
    CGRenderObjectProperty* GetRenderProperty() noexcept;
    
    const CGRenderObjectProperty* GetRenderProperty() const noexcept;

    /**
     * @brief Is the component visual in the game.
     */
    bool visual = true;

    /**
     * @brief Draw component on the window.
     */
    virtual void Draw(float p_delta) = 0;
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
     * @brief Set the Visual object
     * 
     * @param p_visual The new value of visual
     */
    void SetVisual(bool p_visual) noexcept;

    /**
     * @brief Is the component visual in the game
     * 
     * @return true The component is visual in the game
     * @return false The component is not visual in the game
     */
    bool IsVisual() const noexcept;
};