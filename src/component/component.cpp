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
        return 0;
    float top_y = 0;
    for (auto& child : children)
    {
        float child_top_y = child->GetBoarderTopY() * transform.scale.y;
        if (child_top_y > top_y)
            top_y = child_top_y;
    }
    return top_y;
}

float CGComponent::GetBoarderBottomY() const noexcept
{
    if (children.empty())
        return 0;
    float bottom_y = 0;
    for (auto& child : children)
    {
        float child_bottom_y = child->GetBoarderBottomY() * transform.scale.y;
        if (child_bottom_y < bottom_y)
            bottom_y = child_bottom_y;
    }
    return bottom_y;
}

float CGComponent::GetBoarderLeftX() const noexcept
{
    if (children.empty())
        return 0;
    float left_x = 0;
    for (auto& child : children)
    {
        float child_left_x = child->GetBoarderLeftX() * transform.scale.x;
        if (child_left_x < left_x)
            left_x = child_left_x;
    }
    return left_x;
}

float CGComponent::GetBoarderRightX() const noexcept
{
    if (children.empty())
        return 0;
    float right_x = 0;
    for (auto& child : children)
    {
        float child_right_x = child->GetBoarderRightX() * transform.scale.x;
        if (child_right_x > right_x)
            right_x = child_right_x;
    }
    return right_x;
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


void CGComponent::AllignTop(float p_offset)
{
    transform.position.y = CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderTopY() + p_offset;
}

void CGComponent::AllignBottom(float p_offset)
{
    transform.position.y = -1 * CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderBottomY() + p_offset;
}

void CGComponent::AllignLeft(float p_offset)
{
    transform.position.x = -1 * CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderLeftX() + p_offset;
}

void CGComponent::AllignRight(float p_offset)
{
    transform.position.x = CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderRightX() + p_offset;
}

void CGComponent::AllignBottomToTop(const CGIRectBoarder& target, float p_offset)
{
    transform.position.y = target.GetBoarderTopY() - GetBoarderBottomY() / 2.0f + p_offset;
}

void CGComponent::AllignTopToBottom(const CGIRectBoarder& target, float p_offset)
{
    transform.position.y = target.GetBoarderBottomY() - GetBoarderTopY() / 2.0f + p_offset;
}

void CGComponent::AllignLeftToRight(const CGIRectBoarder& target, float p_offset)
{
    transform.position.x = target.GetBoarderRightX() - GetBoarderLeftX() / 2.0f + p_offset;
}

void CGComponent::AllignRightToLeft(const CGIRectBoarder& target, float p_offset)
{
    transform.position.x = target.GetBoarderLeftX() - GetBoarderRightX() / 2.0f + p_offset;
}