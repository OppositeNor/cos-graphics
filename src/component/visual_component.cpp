#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/game.h"

CGVisualComponent::CGVisualComponent() : CGComponent()
{
    render_property = CGCreateRenderObjectProperty(CGConstructColor(1.0f, 1.0f, 1.0f,1.0f), 
        transform.position, transform.scale, transform.rotation);
}

CGVisualComponent::CGVisualComponent(const CGVisualComponent& p_other) : CGComponent(p_other)
{
    render_property = CGCreateRenderObjectProperty(p_other.render_property->color, 
        p_other.transform.position, p_other.transform.scale, p_other.transform.rotation);
    visual = p_other.visual;
}

CGVisualComponent::~CGVisualComponent()
{
}

void CGVisualComponent::Tick(double p_delta_time)
{
    CGComponent::Tick(p_delta_time);
    if (visual)
        Draw(p_delta_time);
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
