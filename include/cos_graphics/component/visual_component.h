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
     * @brief The window that the component will be drawn on.
     */
    CGWindow* window;
    
    /**
     * @brief The position on the window that the component will be drawn.
     */
    CGVector2 position;

    /**
     * @brief Draw component on the window.
     */
    virtual void Draw() = 0;
public:
    CGVisualComponent(const CGVector2& p_position = {0, 0});
    virtual ~CGVisualComponent();

    /**
     * @brief Called every frame by the engine
     * 
     * @param p_delta_time The time difference between frames
     */
    virtual void Tick(double p_delta_time) override;
};