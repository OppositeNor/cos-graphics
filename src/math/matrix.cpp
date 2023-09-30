#include "cos_graphics/math/matrix.hpp"

namespace CGMat3
{
    CGMat3 GetPositionMatrix(const CGVector2& p_position) noexcept
    {
        return CGMat3({1.0f, 0.0f, p_position.x,
                    0.0f, 1.0f, p_position.y,
                    0.0f, 0.0f, 1.0f});
    }
    
    CGMat3 GetRotationMatrix(float p_rotation) noexcept
    {
        float sin_theta = sin(p_rotation);
        float cos_theta = cos(p_rotation);
        return CGMat3({cos_theta, -sin_theta, 0.0f,
                    sin_theta, cos_theta, 0.0f,
                    0.0f, 0.0f, 1.0f});
    }
    
    CGMat3 GetScaleMatrix(const CGVector2& p_scale) noexcept
    {
        return CGMat3({p_scale.x, 0.0f, 0.0f,
                    0.0f, p_scale.y, 0.0f,
                    0.0f, 0.0f, 1.0f});
    }
}