#include "cos_graphics/component/texture_container.h"

CGTextureContainer::~CGTextureContainer()
{
    if (texture != nullptr)
        CGFree(texture);
}


float CGTextureContainer::GetBoarderWidth() const noexcept
{
    if (GetChildren().empty())
        return texture->img_width * transform.scale.x;
    return GetBoarderRightX() - GetBoarderLeftX();
}

float CGTextureContainer::GetBoarderHeight() const noexcept
{
    if (GetChildren().empty())
        return texture->img_height * transform.scale.y;
    return GetBoarderTopY() - GetBoarderBottomY();
}

float CGTextureContainer::GetBoarderTopY() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return (float)texture->img_height * transform.scale.y / 2.0f;
    float top_y = (float)texture->img_height * transform.scale.y / 2.0f;
    for (auto& child : children)
    {
        float child_top_y = (child->GetTransform().position.y + child->GetBoarderTopY()) * transform.scale.y;
        if (child_top_y > top_y)
            top_y = child_top_y;
    }
    return top_y;
}

float CGTextureContainer::GetBoarderBottomY() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return (float)texture->img_height * transform.scale.y / -2.0f;
    float bottom_y = (float)texture->img_height * transform.scale.y / -2.0f;
    for (auto& child : children)
    {
        float child_bottom_y = (child->GetTransform().position.y + child->GetBoarderBottomY()) * transform.scale.y;
        if (child_bottom_y < bottom_y)
            bottom_y = child_bottom_y;
    }
    return bottom_y;
}

float CGTextureContainer::GetBoarderLeftX() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return (float)texture->img_width * transform.scale.x / -2.0f;
    float left_x = (float)texture->img_width * transform.scale.x / -2.0f;
    for (auto& child : children)
    {
        float child_left_x = (child->GetTransform().position.x + child->GetBoarderLeftX()) * transform.scale.x;
        if (child_left_x < left_x)
            left_x = child_left_x;
    }
    return left_x;
}

float CGTextureContainer::GetBoarderRightX() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return (float)texture->img_width * transform.scale.x / 2.0f;
    float right_x = (float)texture->img_width * transform.scale.x / 2.0f;
    for (auto& child : children)
    {
        float child_right_x = (child->GetTransform().position.x + child->GetBoarderRightX()) * transform.scale.x;
        if (child_right_x > right_x)
            right_x = child_right_x;
    }
    return right_x;
}

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
    return texture->img_width * transform.scale.x;
}

float CGTextureContainer::GetHeight() const noexcept
{
    return texture->img_height * transform.scale.y;
}

float CGTextureContainer::GetTopY() const noexcept
{
    return transform.position.y + (float)texture->img_height * transform.scale.y / 2.0f;
}

float CGTextureContainer::GetBottomY() const noexcept
{
    return transform.position.y - (float)texture->img_height * transform.scale.y / 2.0f;
}

float CGTextureContainer::GetLeftX() const noexcept 
{
    return transform.position.x - (float)texture->img_width * transform.scale.x / 2.0f;
}

float CGTextureContainer::GetRightX() const noexcept
{
    return transform.position.x + (float)texture->img_width * transform.scale.x / 2.0f;
}