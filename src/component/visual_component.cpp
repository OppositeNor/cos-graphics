#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/game.h"

CGVisualComponent::CGVisualComponent(const CGVector2& p_position) : CGComponent(), position(p_position)
{
}

CGVisualComponent::~CGVisualComponent()
{
}

void CGVisualComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
    if (visual)
        Draw();
}

void CGVisualComponent::SetVisual(bool p_visual)
{
    visual = p_visual;
}

bool CGVisualComponent::IsVisual()
{
    return visual;
}


void CGVisualComponent::SetDepth(float p_depth)
{
    transform.depth = p_depth;
}
