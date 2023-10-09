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

static inline CGVector2& operator*=(CGVector2& p_vec, float p_num) noexcept
{
    p_vec.x *= p_num;
    p_vec.y *= p_num;
    return p_vec;
}

static inline CGVector2 operator*(float p_num, const CGVector2& p_vec) noexcept
{
    return {p_vec.x * p_num, p_vec.y * p_num};
}

static inline CGVector2 operator/(const CGVector2& p_vec, float p_num)
{
    return {p_vec.x / p_num, p_vec.y / p_num};
}

/**
 * @brief A constructor for CGVector2.
 * 
 * @param x The x value of the vector.
 * @param y The y value of the vector.
 * @return CGVector2 
 */
static inline CGVector2 CGVec2(float x, float y)
{
    return (CGVector2){x, y};
}

/**
 * @brief The length of a vector.
 * 
 * @param p_vec The vector to get the length from.
 * @return float The length of the vector.
 */
static inline float length(const CGVector2& p_vec) noexcept
{
    return sqrt(p_vec.x * p_vec.x + p_vec.y * p_vec.y);
}

/**
 * @brief The normalized vector of a vector.
 * @details A normalized vector is a vector with the same direction but with length 1.
 * @param p_vec The vector to be normalized.
 * @return CGVector2 The normalized vector.
 */
static inline CGVector2 normalize(const CGVector2& p_vec) noexcept
{
    if (p_vec.x == 0 && p_vec.y == 0)
        return CGVec2(0.0f, 0.0f);
    float length = sqrt(p_vec.x * p_vec.x + p_vec.y * p_vec.y);
    return CGVec2(p_vec.x / length, p_vec.y / length);
}

static inline CGColor operator*(const CGColor& p_color, float p_num) noexcept
{
    return {p_color.r * p_num, p_color.g * p_num, p_color.b * p_num, p_color.alpha * p_num};
}

static inline CGColor& operator*=(CGColor& p_color, float p_num) noexcept
{
    p_color.r *= p_num;
    p_color.g *= p_num;
    p_color.b *= p_num;
    p_color.alpha *= p_num;
    return p_color;
}

static inline CGColor operator*(const CGColor& p_color1, const CGColor& p_color2) noexcept
{
    return {p_color1.r * p_color2.r, p_color1.g * p_color2.g, p_color1.b * p_color2.b, p_color1.alpha * p_color2.alpha};
}

static inline CGColor& operator*=(CGColor& p_color1, const CGColor& p_color2) noexcept
{
    p_color1.r *= p_color2.r;
    p_color1.g *= p_color2.g;
    p_color1.b *= p_color2.b;
    p_color1.alpha *= p_color2.alpha;
    return p_color1;
}
