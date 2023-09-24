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

CGVisualComponent::CGVisualComponent(CGVisualComponent&& p_other) : CGComponent(std::move(p_other))
{
    render_property = p_other.render_property;
    p_other.render_property = nullptr;
    visual = p_other.visual;
}

CGVisualComponent::~CGVisualComponent()
{
    CGFree(render_property);
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

void CGVisualComponent::AllignTop(float p_offset)
{
    transform.position.y = CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderHeight() / 2.0f + p_offset;
}

void CGVisualComponent::AllignBottom(float p_offset)
{
    transform.position.y = -1 * CGGame::GetInstance()->GetGameWindow()->height / 2.0f + GetBoarderHeight() / 2.0f + p_offset;
}

void CGVisualComponent::AllignLeft(float p_offset)
{
    transform.position.x = -1 * CGGame::GetInstance()->GetGameWindow()->width / 2.0f + GetBoarderWidth() / 2.0f + p_offset;
}

void CGVisualComponent::AllignRight(float p_offset)
{
    transform.position.x = CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderWidth() / 2.0f + p_offset;
}

void CGVisualComponent::AllignBottomToTop(const CGIRectBoarder& target, float p_offset)
{
    transform.position.y = target.GetBoarderTopY() + GetBoarderHeight() / 2.0f + p_offset;
}

void CGVisualComponent::AllignTopToBottom(const CGIRectBoarder& target, float p_offset)
{
    transform.position.y = target.GetBoarderBottomY() - GetBoarderHeight() / 2.0f + p_offset;
}

void CGVisualComponent::AllignLeftToRight(const CGIRectBoarder& target, float p_offset)
{
    transform.position.x = target.GetBoarderRightX() + GetBoarderWidth() / 2.0f + p_offset;
}

void CGVisualComponent::AllignRightToLeft(const CGIRectBoarder& target, float p_offset)
{
    transform.position.x = target.GetBoarderLeftX() - GetBoarderWidth() / 2.0f + p_offset;
}

void CGVisualComponent::SetColor(const CGColor& p_color) noexcept
{
    render_property->color = p_color;
}