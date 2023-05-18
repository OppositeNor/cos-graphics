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
    Draw();
    has_transform_set = false;
}

void CGVisualComponent::SetVisual(bool p_visual)
{
    visual = p_visual;
}

bool CGVisualComponent::IsVisual()
{
    return visual;
}