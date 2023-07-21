#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/component/camera.h"
#include "cos_graphics/utils.hpp"
#include "cos_graphics/game.h"

CGVisualComponent::CGVisualComponent() : CGComponent()
{
    render_property = CGCreateRenderObjectProperty(CGConstructColor(1.0f, 1.0f, 1.0f,1.0f), 
        transform.position, transform.scale, transform.rotation);
}

CGRenderObjectProperty* CGVisualComponent::GetRenderProperty() noexcept
{
    return render_property;
}

const CGRenderObjectProperty* CGVisualComponent::GetRenderProperty() const noexcept
{
    return render_property;
}

CGVisualComponent::CGVisualComponent(const CGVisualComponent& p_other) : CGComponent(p_other)
{
    render_property = CGCreateRenderObjectProperty(p_other.render_property->color, 
        p_other.transform.position, p_other.transform.scale, p_other.transform.rotation);
    visual = p_other.visual;
}

CGVisualComponent::~CGVisualComponent()
{
    CGFreeResource(render_property);
}

void CGVisualComponent::Tick(double p_delta_time)
{
    CGComponent::Tick(p_delta_time);
    render_property->z = transform.depth;

    CGTransform global_transform = GetGlobalTransform();
    render_property->transform = global_transform.position;
    if (CGGame::GetInstance()->GetMainCamera() != nullptr)
        render_property->transform -= CGGame::GetInstance()->GetMainCamera()->GetTransform().position;
    render_property->rotation = global_transform.rotation;
    render_property->scale = global_transform.scale;
    
    if (visual)
        Draw(p_delta_time);
}

void CGVisualComponent::SetVisual(bool p_visual) noexcept
{
    visual = p_visual;
}

bool CGVisualComponent::IsVisual() const noexcept
{
    return visual;
}