#pragma once
#include <initializer_list>
#include <cmath>
#include <string>
#include "cos_graphics/graphics.h"
#include "cos_graphics/game.h"
#include "cos_graphics/component/component.h"

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
    static constexpr auto GetReciprocal(T&& p_num) -> decltype(1.0 / p_num)
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
    static std::vector<CGVisualImage*> CreateVisualImageList(const std::initializer_list<CGString>& p_image_rks)
    {
        std::vector<CGVisualImage*> list;
        for (auto iter : p_image_rks)
        {
            list.insert(list.end(), CGCreateVisualImage(iter.c_str(), CGGame::GetInstance()->GetGameWindow()));
        }
        return list;
    }

    /**
     * @brief Get the position of a vector which rotated by a certain angle according to a center.
     * 
     * @param p_position The position of the vector to be rotated.
     * @param p_rotation The angle to rotate.
     * @param p_center The center of the rotation.
     * @return CGVector2 The position of the vector after rotation.
     */
    static inline CGVector2 GetVectorRotatedPosition(const CGVector2& p_position, float p_rotation, const CGVector2 p_center)
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

    static inline float Dot(const CGVector2& p_vec_1, const CGVector2& p_vec_2) noexcept
    {
        return p_vec_1.x * p_vec_2.x + p_vec_1.y * p_vec_2.y;
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

static inline CGVector2 operator*(const CGVector2& p_vec, float p_num) noexcept
{
    return {p_vec.x * p_num, p_vec.y * p_num};
}

static inline CGVector2 operator*(float p_num, const CGVector2& p_vec) noexcept
{
    return {p_vec.x * p_num, p_vec.y * p_num};
}

static inline CGVector2 operator/(const CGVector2& p_vec, float p_num)
{
    return {p_vec.x / p_num, p_vec.y / p_num};
}