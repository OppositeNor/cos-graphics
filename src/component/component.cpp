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
        CGVec2(CGUtils::GetReciprocal(scale.x), CGUtils::GetReciprocal(scale.y))) 
        * CGMat3::GetInvRoationMatrix(rotation)
        * CGMat3::GetPositionMatrix(-1 * position);
}

void CGComponent::OnEnter()
{
    if (activated)
        return;
    for (auto& child : children)
    {
        child->OnEnter();
    }
    activated = true;
    Ready();
}

void CGComponent::QueueFree()
{
    is_queue_freed = true;
}

void CGComponent::ShouldUpdateMatrix()
{
    should_update_global_matrix = true;
    should_update_inv_global_matrix = true;
    for (auto child : children)
    {
        if (!child->should_update_global_matrix || !child->should_update_inv_global_matrix)
            child->ShouldUpdateMatrix();
    }
}

CGComponent::CGTransform& CGComponent::GetTransform() noexcept 
{
    ShouldUpdateMatrix();
    return transform;
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
    for (auto& child : children)
    {
        DetachChild(child);
    }
    if (parent != nullptr)
        parent->DetachChild(this);
    CGGame::GetInstance()->RemoveComponent(this);
}

#define CGBoarderFunc(Direction, AXIS, axis, comp)                          \
float CGComponent::GetBoarder##Direction##AXIS() noexcept                   \
{                                                                           \
    if (children.empty())                                                   \
        return GetGlobalPosition().y;                                       \
    float result = GetGlobalPosition().y;                                   \
    for (auto& child : children)                                            \
    {                                                                       \
        float child_direct_axis = child->GetBoarder##Direction##AXIS();     \
        if (child_direct_axis comp result)                                  \
            result = child_direct_axis;                                     \
    }                                                                       \
    return result;                                                          \
}


void CGComponent::SetVisible(bool p_visible) noexcept
{
    visible = p_visible;
}

bool CGComponent::IsVisible() const noexcept
{
    if (!visible)
        return false;
    if (GetParent() == nullptr)
        return true;
    return GetParent()->IsVisible();
}

bool CGComponent::GetVisible() const noexcept
{
    return visible;
}

CGBoarderFunc(Top, Y, y, >)
CGBoarderFunc(Bottom, Y, y, <)
CGBoarderFunc(Left, X, x, <)
CGBoarderFunc(Right, X, x, >)

void CGComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
}

CGVector2 CGComponent::GetGlobalPosition() const
{
    if (parent == nullptr)
        return GetTransform().position;
    return parent->GetGlobalTransformMatrix() * GetTransform().position;
}

CGMat3::CGMat3 CGComponent::GetGlobalTransformMatrix() noexcept
{
    if (parent == nullptr)
        return GetTransform().GetTransformMatrix();
    if (should_update_global_matrix)
    {
        global_transform_matrix = parent->GetGlobalTransformMatrix() * GetTransform().GetTransformMatrix();
        should_update_global_matrix = false;
    }
    return global_transform_matrix;
}

CGMat3::CGMat3 CGComponent::GetInvGlobalTransformMatrix() noexcept
{
    if (parent == nullptr)
        return GetTransform().GetInvTransformMatrix();
    if (should_update_inv_global_matrix)
    {
        inv_global_transform_matrix = parent->GetInvGlobalTransformMatrix() * GetTransform().GetInvTransformMatrix();
        should_update_inv_global_matrix = false;
    }
    return inv_global_transform_matrix;
}

CGVector2 CGComponent::ToRelativePosition(const CGVector2& global_position) const
{
    if (parent == nullptr)
        return global_position;
    return parent->GetInvGlobalTransformMatrix() * GetTransform().GetInvTransformMatrix() * global_position;
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
        GetTransform().position = global_position;
    else
        GetTransform().position = parent->GetInvGlobalTransformMatrix() * global_position;
}

void CGComponent::SetDepth(float p_depth)
{
    GetTransform().depth = p_depth;
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
    if (activated)
        p_child->OnEnter();
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
    if (p_parent->activated)
        OnEnter();
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


void CGComponent::AlignTop(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = CGGame::GetInstance()->GetGameWindow()->height / 2.0f - GetBoarderTopY() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignBottom(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = CGGame::GetInstance()->GetGameWindow()->height / -2.0f - GetBoarderBottomY() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignLeft(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = CGGame::GetInstance()->GetGameWindow()->width / -2.0f - GetBoarderLeftX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignRight(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderRightX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignBottomToTop(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderTopY() + GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignTopToTop(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderTopY() - GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignTopToBottom(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderBottomY() - GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignBottomToBottom(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderBottomY() + GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignLeftToRight(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderRightX() + GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignRightToRight(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderRightX() - GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignRightToLeft(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderLeftX() - GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AlignLeftToLeft(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderLeftX() + GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}