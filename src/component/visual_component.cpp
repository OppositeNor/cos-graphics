#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/component/camera.h"
#include "cos_graphics/utils.hpp"
#include "cos_graphics/game.h"

CGVisualComponent::CGVisualComponent() : CGComponent()
{
    render_property = CGCreateRenderObjectProperty(CGConstructColor(1.0f, 1.0f, 1.0f,1.0f), 
        GetTransform().position, GetTransform().scale, GetTransform().rotation);
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
        p_other.GetTransform().position, p_other.GetTransform().scale, p_other.GetTransform().rotation);
    visible = p_other.visible;
}

CGVisualComponent::CGVisualComponent(CGVisualComponent&& p_other) : CGComponent(std::move(p_other))
{
    render_property = p_other.render_property;
    p_other.render_property = nullptr;
    visible = p_other.visible;
}

CGVisualComponent::~CGVisualComponent()
{
    CGFree(render_property);
}

void CGVisualComponent::Tick(double p_delta_time)
{
    CGComponent::Tick(p_delta_time);
    if (!IsVisible())
        return;

    render_property->z = GetTransform().depth;

    render_property->transform = GetTransform().position;
    if (CGGame::GetInstance()->GetMainCamera() != nullptr)
        render_property->transform -= CGGame::GetInstance()->GetMainCamera()->GetTransform().position;
    render_property->rotation = GetTransform().rotation;
    render_property->scale = GetTransform().scale;
    if (GetParent() != nullptr)
    {
        auto parent_transform_matrix = GetParent()->GetGlobalTransformMatrix();
        for (unsigned int i = 0; i < 4; ++i)
        {
            for (unsigned int j = 0; j < 4; ++j)
            {
                matrix_buffer[i][j] = (i < 3 && j < 3) ? parent_transform_matrix[j][i] : (i == j ? 1.0f : 0.0f);
            }
        }
        render_property->modify_matrix = (float*)matrix_buffer;
    }
    else
        render_property->modify_matrix = NULL;
    
    Draw(p_delta_time);
}

void CGVisualComponent::SetColor(const CGColor& p_color) noexcept
{
    render_property->color = p_color;
}