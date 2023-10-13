#include "cos_graphics/component/texture_container.h"
#include "cos_graphics/utils.hpp"

#define CGBoarderFunc(Direction, AXIS, axis, comp)                          \
float CGTextureContainer::GetBoarder##Direction##AXIS() noexcept            \
{                                                                           \
    auto& children = GetChildren();                                         \
    CGVector2 result = CGVec2(0.0f, 0.0f);                                  \
    if (texture == nullptr)                                                 \
    {                                                                       \
        if (children.empty())                                               \
            return GetGlobalPosition().axis;                                \
        result.axis = GetGlobalPosition().axis;                             \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        result.axis = Get##Direction##AXIS();                               \
        result = GetGlobalTransformMatrix() * result;                       \
        if (children.empty())                                               \
            return result.axis;                                             \
    }                                                                       \
    for (auto& child : children)                                            \
    {                                                                       \
        float child_direct_axis = child->GetBoarder##Direction##AXIS();     \
        if (child_direct_axis comp result.axis)                             \
            result.axis = child_direct_axis;                                \
    }                                                                       \
    return result.axis;                                                     \
}


CGTextureContainer::~CGTextureContainer()
{
    if (texture != nullptr)
        CGFree(texture);
}


float CGTextureContainer::GetBoarderWidth() noexcept
{
    if (texture == nullptr)
        return 0;
    return GetBoarderRightX() - GetBoarderLeftX();
}

float CGTextureContainer::GetBoarderHeight() noexcept
{
    if (texture == nullptr)
        return 0;
    return GetBoarderTopY() - GetBoarderBottomY();
}

CGBoarderFunc(Top, Y, y, >)
CGBoarderFunc(Bottom, Y, y, <)
CGBoarderFunc(Left, X, x, <)
CGBoarderFunc(Right, X, x, >)

CGTextureContainer::CGTextureContainer(const CGTextureContainer& p_other) : CGVisualComponent(p_other)
{
    texture = CGCopyVisualImage(p_other.texture);
}

CGTextureContainer::CGTextureContainer(CGTextureContainer&& p_other) noexcept : CGVisualComponent(std::move(p_other))
{
    texture = p_other.texture;
    p_other.texture = nullptr;
}

float CGTextureContainer::GetWidth() const noexcept
{
    return texture->img_width * GetTransform().scale.x;
}

float CGTextureContainer::GetHeight() const noexcept
{
    return texture->img_height * GetTransform().scale.y;
}

float CGTextureContainer::GetTopY() const noexcept
{
    return (float)texture->img_height / 2.0f;
}

float CGTextureContainer::GetBottomY() const noexcept
{
    return -1 * (float)texture->img_height / 2.0f;
}

float CGTextureContainer::GetLeftX() const noexcept 
{
    return -1 * (float)texture->img_width / 2.0f;
}

float CGTextureContainer::GetRightX() const noexcept
{
    return (float)texture->img_width / 2.0f;
}