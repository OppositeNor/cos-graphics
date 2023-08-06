#include "cos_graphics/component/component.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/utils.hpp"
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
    if (!CGGame::GetInstance()->IsGameTerminating())
    {
        CGGame::GetInstance()->RemoveComponent(this);
        for (auto& child : children)
        {
            delete child;
        }
        if (parent != nullptr)
            parent->RemoveChild(this);
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

CGVector2 CGComponent::GetGlobalPosition() const
{
    if (parent == nullptr)
        return transform.position;
    const CGTransform& parent_transform = parent->GetTransform();
    CGVector2 global_position = transform.position;
    global_position.x += parent_transform.position.x;
    global_position.y += parent_transform.position.y;
    global_position = CGUtils::GetVectorRotatedPosition(global_position, parent_transform.rotation, parent_transform.position);
    return global_position;
}

CGComponent::CGTransform CGComponent::GetGlobalTransform() const
{
    if (parent == nullptr)
        return transform;
    const CGTransform& parent_transform = parent->GetTransform();
    CGTransform global_transform = parent->GetGlobalTransform();
    global_transform.position.x += transform.position.x * parent_transform.scale.x;
    global_transform.position.y += transform.position.y * parent_transform.scale.y;
    global_transform.rotation += transform.rotation;
    
    global_transform.scale.x *= transform.scale.x;
    global_transform.scale.y *= transform.scale.y;
    global_transform.position = CGUtils::GetVectorRotatedPosition(global_transform.position, parent_transform.rotation, parent_transform.position);
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

void CGComponent::RemoveChild(CGComponent* p_child)
{
    if (p_child == nullptr)
    {
        CG_WARNING("Trying to remove a null child from a component.");
        return;
    }
    for (auto child = children.begin(); child != children.end(); ++child)
    {
        if (*child == p_child)
        {
            children.erase(child);
            p_child->parent = nullptr;
            return;
        }
    }
    CG_WARNING("Trying to remove a child that doesn't exist.");
}

void CGComponent::SetParent(CGComponent* p_parent)
{
    if (p_parent == nullptr)
    {
        CG_WARNING("Trying to set a null parent to a component.");
        return;
    }
    if (parent != nullptr)
        parent->DetachChild(this);
    p_parent->AddChild(this);
}

void CGComponent::DetachChild(CGComponent* p_child)
{
    if (p_child == nullptr)
    {
        CG_WARNING("Trying to detach a null child from a component.");
        return;
    }
    for (auto child = children.begin(); child != children.end(); ++child)
    {
        if (*child == p_child)
        {
            children.erase(child);
            p_child->parent = nullptr;
            return;
        }
    }
    CG_WARNING("Trying to detach a child that doesn't exist.");
}