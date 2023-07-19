#pragma once
#include <initializer_list>
#include <cmath>
#include <string>
#include "cos_graphics/graphics.h"

class CGUtils
{
    CGUtils() = delete;
public:
    /**
     * @brief Get the reciprocal of a number.
     * 
     * @tparam T The type of the number.
     * @param p_num The number to get the reciprocal.
     * @return constexpr T The reciprocal of the number.
     */
    template <typename T>
    static constexpr auto CGGetReciprocal(T&& p_num) -> decltype(1.0 / p_num)
    {
        return 1.0 / p_num;
    }
    
    /**
     * @brief Create a list of CGVisualImage* from a list of image resource keys.
     * @note You should free the element list after using it.
     * 
     * @param p_image_rks The list of image resource keys.
     * @return std::vector<CGVisualImage*>&& The list of CGVisualImage*.
     */
    static std::vector<CGVisualImage*> CGCreateVisualImageList(const std::initializer_list<std::string>& p_image_rks)
    {
        std::vector<CGVisualImage*> list;
        for (auto iter : p_image_rks)
        {
            list.insert(list.end(), CGCreateVisualImage(iter.c_str(), CGGame::GetInstance()->GetGameWindow()));
        }
        return list;
    }

    static inline CGVector2 CGGetVectorRotatedPosition(const CGVector2& p_position, float p_rotation, const CGVector2 p_center) noexcept
    {
        CGVector2 result;
        float sin_theta = sin(p_rotation);
        float cos_theta = cos(p_rotation);
        float delta_x = p_position.x - p_center.x;
        float delta_y = p_position.y - p_center.y;
        result.x = delta_x * cos_theta - delta_y * sin_theta + p_center.x;
        result.y = delta_y * cos_theta + delta_x * sin_theta + p_center.y;
        return result;
    }
};

static constexpr CGVector2 operator+(const CGVector2& p_vec1, const CGVector2& p_vec2) noexcept
{
    return {p_vec1.x + p_vec2.x, p_vec1.y + p_vec2.y};
}

static inline CGVector2& operator+=(CGVector2& p_vec1, const CGVector2& p_vec2) noexcept
{
    p_vec1.x += p_vec2.x;
    p_vec1.y += p_vec2.y;
    return p_vec1;
}

static constexpr CGVector2 operator-(const CGVector2& p_vec1, const CGVector2& p_vec2) noexcept
{
    return {p_vec1.x - p_vec2.x, p_vec1.y - p_vec2.y};
}

static inline CGVector2& operator-=(CGVector2& p_vec1, const CGVector2& p_vec2) noexcept
{
    p_vec1.x -= p_vec2.x;
    p_vec1.y -= p_vec2.y;
    return p_vec1;
}
