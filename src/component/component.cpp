#include "cos_graphics/component/component.h"
#include "cos_graphics/game.h"

CGComponent::CGTransform::CGTransform(const CGVector2& p_position, float p_rotation, const CGVector2& p_scale)
    : position(p_position), rotation(p_rotation), scale(p_scale)
{
    
}

CGComponent::CGComponent()
{
    CGGame::GetInstance()->AddComponent(this);
    Ready();
}

CGComponent::~CGComponent()
{
    CGGame::GetInstance()->RemoveComponent(this);
}

void CGComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
    has_transform_set = false;
}

const CGComponent::CGTransform& CGComponent::GetTransform() const
{
    return transform;
}

void CGComponent::SetTransform(const CGTransform& p_transform)
{
    transform = p_transform;
    has_transform_set = true;
}

void CGComponent::SetPosition(const CGVector2& p_position)
{
    transform.position = p_position;
    has_transform_set = true;
}

void CGComponent::SetRotation(float p_rotation)
{
    transform.rotation = p_rotation;
    has_transform_set = true;
}

void CGComponent::SetScale(const CGVector2& p_scale)
{
    transform.scale = p_scale;
    has_transform_set = true;
}
