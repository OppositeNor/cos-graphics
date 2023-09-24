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
    float max_right = 0.0f;
    float min_left = 0.0f;
    for (auto& child : GetChildren())
    {
        float child_right = child->GetBoarderRightX();
        float child_left = child->GetBoarderLeftX();
        if (child_right > max_right)
            max_right = child_right;
        if (child_left < min_left)
            min_left = child_left;
    }
    return (max_right - min_left) * transform.scale.x;
}

float CGTextureContainer::GetBoarderHeight() const noexcept
{
    if (GetChildren().empty())
        return texture->img_height * transform.scale.y;
    float max_top = 0.0f;
    float min_bottom = 0.0f;
    for (auto& child : GetChildren())
    {
        float child_top = child->GetBoarderTopY();
        float child_bottom = child->GetBoarderBottomY();
        if (child_top > max_top)
            max_top = child_top;
        if (child_bottom < min_bottom)
            min_bottom = child_bottom;
    }
    return (max_top - min_bottom) * transform.scale.y;
}

float CGTextureContainer::GetBoarderTopY() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return transform.position.y + texture->img_height * transform.scale.y / 2.0f;
    float top_y = children[0]->GetBoarderTopY();
    for (auto& child : children)
    {
        float child_top_y = child->GetBoarderTopY();
        if (child_top_y > top_y)
            top_y = child_top_y;
    }
    return top_y * transform.scale.y;
}

float CGTextureContainer::GetBoarderBottomY() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return transform.position.y - texture->img_height * transform.scale.y / 2.0f;
    float bottom_y = children[0]->GetBoarderBottomY();
    for (auto& child : children)
    {
        float child_bottom_y = child->GetBoarderBottomY();
        if (child_bottom_y < bottom_y)
            bottom_y = child_bottom_y;
    }
    return bottom_y * transform.scale.y;
}

float CGTextureContainer::GetBoarderLeftX() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return transform.position.x - texture->img_width * transform.scale.x / 2.0f;
    float left_x = children[0]->GetBoarderLeftX();
    for (auto& child : children)
    {
        float child_left_x = child->GetBoarderLeftX();
        if (child_left_x < left_x)
            left_x = child_left_x;
    }
    return left_x * transform.scale.x;
}

float CGTextureContainer::GetBoarderRightX() const noexcept
{
    auto& children = GetChildren();
    if (children.empty())
        return transform.position.x + texture->img_width * transform.scale.x / 2.0f;
    float right_x = children[0]->GetBoarderRightX();
    for (auto& child : children)
    {
        float child_right_x = child->GetBoarderRightX();
        if (child_right_x > right_x)
            right_x = child_right_x;
    }
    return right_x * transform.scale.x;
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