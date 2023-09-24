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
}

CGComponent::CGComponent(const CGComponent& other) : CGComponent()
{
    transform = other.transform;
}

CGComponent::CGComponent(CGComponent&& other) noexcept : CGComponent()
{
    transform = other.transform;
    parent = other.parent;
    other.parent = nullptr;
    children = std::move(other.children);
}

CGComponent::~CGComponent()
{
    CGGame::GetInstance()->RemoveComponent(this);
    for (auto& child : children)
    {
        DetachChild(child);
    }
    if (parent != nullptr)
        parent->DetachChild(this);
}

float CGComponent::GetBoarderTopY() const noexcept
{
    if (children.empty())
        return transform.position.y;
    float top_y = children[0]->GetBoarderTopY();
    for (auto& child : children)
    {
        float child_top_y = child->GetBoarderTopY();
        if (child_top_y > top_y)
            top_y = child_top_y;
    }
    return top_y * transform.scale.y;
}

float CGComponent::GetBoarderBottomY() const noexcept
{
    if (children.empty())
        return transform.position.y;
    float bottom_y = children[0]->GetBoarderBottomY();
    for (auto& child : children)
    {
        float child_bottom_y = child->GetBoarderBottomY();
        if (child_bottom_y < bottom_y)
            bottom_y = child_bottom_y;
    }
    return bottom_y * transform.scale.y;
}

float CGComponent::GetBoarderLeftX() const noexcept
{
    if (children.empty())
        return transform.position.x;
    float left_x = children[0]->GetBoarderLeftX();
    for (auto& child : children)
    {
        float child_left_x = child->GetBoarderLeftX();
        if (child_left_x < left_x)
            left_x = child_left_x;
    }
    return left_x * transform.scale.x;
}

float CGComponent::GetBoarderRightX() const noexcept
{
    if (children.empty())
        return transform.position.x;
    float right_x = children[0]->GetBoarderRightX();
    for (auto& child : children)
    {
        float child_right_x = child->GetBoarderRightX();
        if (child_right_x > right_x)
            right_x = child_right_x;
    }
    return right_x * transform.scale.x;
}

void CGComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
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
        CG_WARNING(CGSTR("Trying to add a null child to a component."));
        return;
    }
    for (auto& child : children)
    {
        if (child == p_child)
        {
            CG_WARNING(CGSTR("Trying to add a child that already exists."));
            return;
        }
    }
    children.push_back(p_child);
    p_child->parent = this;
}

void CGComponent::SetParent(CGComponent* p_parent)
{
    if (p_parent == nullptr)
    {
        CG_WARNING(CGSTR("Trying to set a null parent to a component."));
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
        CG_WARNING(CGSTR("Trying to detach a null child from a component."));
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
    CG_WARNING(CGSTR("Trying to detach a child that doesn't exist."));
}

CGComponent* CGComponent::GetParent() const noexcept
{
    return parent;
}

const std::vector<CGComponent*>& CGComponent::GetChildren() const noexcept
{
    return children;
}