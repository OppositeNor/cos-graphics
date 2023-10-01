#include "cos_graphics/component/component.h"
#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/utils.hpp"
#include <cmath>

CGComponent::CGTransform::CGTransform(const CGVector2& p_position, float p_rotation, const CGVector2& p_scale)
    : position(p_position), rotation(p_rotation), scale(p_scale)
{
    
}

CGMat3::CGMat3 CGComponent::CGTransform::GetTransformMatrix() const noexcept
{
    return CGMat3::GetPositionMatrix(position) * CGMat3::GetRotationMatrix(rotation) * CGMat3::GetScaleMatrix(scale);
}

CGMat3::CGMat3 CGComponent::CGTransform::GetInvTransformMatrix() const noexcept
{
    return CGMat3::GetScaleMatrix(
        CGConstructVector2(CGUtils::GetReciprocal(scale.x), CGUtils::GetReciprocal(scale.y))) 
        * CGMat3::GetRotationMatrix(-rotation) 
        * CGMat3::GetPositionMatrix(CGConstructVector2(- position.x, -position.y ));
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
        return GetGlobalPosition().y;
    float top_y = 0;
    for (auto& child : children)
    {
        float child_top_y = child->GetTransform().position.y + child->GetBoarderTopY() * transform.scale.y;
        if (child_top_y > top_y)
            top_y = child_top_y;
    }
    top_y += GetGlobalPosition().y;
    return top_y;
}

float CGComponent::GetBoarderBottomY() const noexcept
{
    if (children.empty())
        return GetGlobalPosition().y;
    float bottom_y = 0;
    for (auto& child : children)
    {
        float child_bottom_y = child->GetTransform().position.y - child->GetBoarderBottomY() * transform.scale.y;
        if (child_bottom_y < bottom_y)
            bottom_y = child_bottom_y;
    }
    bottom_y += GetGlobalPosition().y;
    return bottom_y;
}

float CGComponent::GetBoarderLeftX() const noexcept
{
    if (children.empty())
        return GetGlobalPosition().x;
    float left_x = 0;
    for (auto& child : children)
    {
        float child_left_x = child->GetTransform().position.x - child->GetBoarderLeftX() * transform.scale.x;
        if (child_left_x < left_x)
            left_x = child_left_x;
    }
    left_x += GetGlobalPosition().x;
    return left_x;
}

float CGComponent::GetBoarderRightX() const noexcept
{
    if (children.empty())
        return GetGlobalPosition().x;
    float right_x = 0;
    for (auto& child : children)
    {
        float child_right_x = child->GetTransform().position.x + child->GetBoarderRightX() * transform.scale.x;
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
    return parent->GetGlobalTransformMatrix() * transform.position;
}

CGMat3::CGMat3 CGComponent::GetGlobalTransformMatrix() const noexcept
{
    if (parent == nullptr)
        return transform.GetTransformMatrix();
    return parent->GetGlobalTransformMatrix() * transform.GetTransformMatrix();
}

CGMat3::CGMat3 CGComponent::GetGlobalInvTransformMatrix() const noexcept
{
    if (parent == nullptr)
        return transform.GetInvTransformMatrix();
    return transform.GetInvTransformMatrix() * parent->GetGlobalInvTransformMatrix();
}

CGVector2 CGComponent::ToRelativePosition(const CGVector2& global_position) const
{
    if (parent == nullptr)
        return global_position;
    return parent->GetGlobalInvTransformMatrix() * transform.GetInvTransformMatrix() * global_position;
}

CGVector2 CGComponent::ToGlobalPosition(const CGVector2& relative_position) const
{
    if (parent == nullptr)
        return relative_position;
    return parent->GetGlobalTransformMatrix() * relative_position;
}

void CGComponent::SetGlobalPosition(const CGVector2& global_position)
{
    if (parent == nullptr)
        transform.position = global_position;
    else
        transform.position = parent->GetGlobalInvTransformMatrix() * global_position;
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
    auto global_position = GetGlobalPosition();
    global_position.y = CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderTopY() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignBottom(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = -1 * CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderBottomY() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignLeft(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = -1 * CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderLeftX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignRight(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderRightX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignBottomToTop(const CGIRectBoarder* target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = target->GetBoarderTopY() + GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignTopToBottom(const CGIRectBoarder* target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = target->GetBoarderBottomY() - GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignLeftToRight(const CGIRectBoarder* target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = target->GetBoarderRightX() + GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignRightToLeft(const CGIRectBoarder* target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = target->GetBoarderLeftX() - GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}