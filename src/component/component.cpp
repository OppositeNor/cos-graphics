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
        * CGMat3::GetInvRoationMatrix(rotation)
        * CGMat3::GetPositionMatrix(-1 * position);
}

void CGComponent::ShouldUpdateMatrix()
{
    should_update_matrix = true;
    for (auto child : children)
    {
        if (!child->should_update_matrix)
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
    CGGame::GetInstance()->RemoveComponent(this);
    for (auto& child : children)
    {
        DetachChild(child);
    }
    if (parent != nullptr)
        parent->DetachChild(this);
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
    if (should_update_matrix)
    {
        global_transform_matrix = parent->GetGlobalTransformMatrix() * GetTransform().GetTransformMatrix();
        should_update_matrix = false;
    }
    return global_transform_matrix;
}

CGMat3::CGMat3 CGComponent::GetGlobalInvTransformMatrix() const noexcept
{
    if (parent == nullptr)
        return GetTransform().GetInvTransformMatrix();
    return GetTransform().GetInvTransformMatrix() * parent->GetGlobalInvTransformMatrix();
}

CGVector2 CGComponent::ToRelativePosition(const CGVector2& global_position) const
{
    if (parent == nullptr)
        return global_position;
    return parent->GetGlobalInvTransformMatrix() * GetTransform().GetInvTransformMatrix() * global_position;
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
        GetTransform().position = parent->GetGlobalInvTransformMatrix() * global_position;
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
    global_position.y = CGGame::GetInstance()->GetGameWindow()->height / -2.0f - GetBoarderBottomY() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignLeft(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = CGGame::GetInstance()->GetGameWindow()->width / -2.0f - GetBoarderLeftX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignRight(float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = CGGame::GetInstance()->GetGameWindow()->width / 2.0f - GetBoarderRightX() + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignBottomToTop(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderTopY() + GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignTopToTop(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.y = p_target->GetBoarderTopY() - GetBoarderHeight() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignTopToBottom(CGIRectBoarder* p_target, float p_offset)
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

void CGComponent::AllignLeftToRight(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderRightX() + GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignRightToRight(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderRightX() - GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignRightToLeft(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderLeftX() - GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}

void CGComponent::AllignLeftToLeft(CGIRectBoarder* p_target, float p_offset)
{
    auto global_position = GetGlobalPosition();
    global_position.x = p_target->GetBoarderLeftX() + GetBoarderWidth() / 2.0f + p_offset;
    SetGlobalPosition(global_position);
}