#include "cos_graphics/component/component.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include <cmath>

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
    for (auto& child : children)
    {
        delete child;
    }
}

void CGComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
}

const CGComponent::CGTransform& CGComponent::GetTransform() const noexcept
{
    return transform;
}

CGComponent::CGTransform& CGComponent::GetTransform() noexcept
{
    return transform;
}

CGComponent::CGTransform CGComponent::GetGlobalTransform() const
{
    if (parent == nullptr)
        return transform;
    CGTransform global_transform = parent->GetGlobalTransform();
    global_transform.position += transform.position;

    return global_transform;
}

void CGComponent::SetDepth(float p_depth)
{
    transform.depth = p_depth;
}

void CGComponent::AddChild(CGComponent* p_child)
{
    if (p_child == nullptr)
    {
        CG_WARNING("Trying to add a null child to a component.");
        return;
    }
    for (auto& child : children)
    {
        if (child == p_child)
        {
            CG_WARNING("Trying to add a child that already exists.");
            return;
        }
    }
    children.push_back(p_child);
    p_child->parent = this;
}