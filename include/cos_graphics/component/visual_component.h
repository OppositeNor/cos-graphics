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
public:
    CGVisualComponent(CGWindow* p_window, CGVector2 p_position = {0, 0});
    virtual ~CGVisualComponent();

    /**
     * @brief Draw component on the window.
     */
    virtual void Draw() = 0;

    /**
     * @brief Resize the component.
     */
    virtual void Resize() = 0;
};