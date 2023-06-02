#pragma once
#include "cos_graphics/component/component.h"
#include "cos_graphics/graphics.h"

/**
 * @brief Visual component is the base class for all objects that are visual in the game world.
 * 
 */
class CGVisualComponent : public CGComponent
{
protected:
    /**
     * @brief The render property for objects to render.
     */
    CGRenderObjectProperty* render_property;

    /**
     * @brief Is the component visual in the game.
     */
    bool visual = true;
    
    /**
     * @brief The position on the window that the component will be drawn.
     */
    CGVector2 position;

    /**
     * @brief Draw component on the window.
     */
    virtual void Draw(float p_delta) = 0;
public:
    explicit CGVisualComponent(const CGVector2& p_position = {0, 0});
    virtual ~CGVisualComponent() override;

    /**
     * @brief Called every frame by the engine
     * 
     * @param p_delta_time The time difference between frames
     */
    void Tick(double p_delta_time) override;

    /**
     * @brief Set the Visual object
     * 
     * @param p_visual The new value of visual
     */
    void SetVisual(bool p_visual);

    /**
     * @brief Is the component visual in the game
     * 
     * @return true The component is visual in the game
     * @return false The component is not visual in the game
     */
    bool IsVisual();
    
    /**
     * @brief Set the depth of the component
     * 
     * @param p_depth The depth that the component will be set to.
     */
    void SetDepth(float p_depth);
};