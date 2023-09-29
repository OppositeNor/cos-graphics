#pragma once
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <cmath>

template<unsigned int N>
class CGMatrix{

public:
    static constexpr int SIZE = N * N;
    static constexpr int DIM = N;


private:
    union {
        float data[SIZE];
        float data_m[N][N];
    };
    inline static CGMatrix<N> IDENTITY = CGMatrix<N>();
    static float CGMatrix_buffer[SIZE];
    static inline bool is_identity_initialized = false;

public:
    /**
     * @brief Constructor
     */
    CGMatrix() noexcept
    {}

    CGMatrix(const float* data, int size)
    {
        CG_ERROR_COND_EXIT(size != SIZE, -1, CGSTR("The size of the data is not equal to the size of the CGMatrix"));
        for (int i = 0; i < size; ++i)
        {
            this->data[i] = data[i];
        }
    }

    /**
     * @brief Constructor
     * @param list The list of values in the CGMatrix (row-major order)
     */
    CGMatrix(const std::initializer_list<float>& list)
    {
        CG_ERROR_COND_EXIT(list.size() != SIZE, -1, CGSTR("The size of the initializer list is not equal to the size of the CGMatrix"));
        int i = 0;
        for (auto iter = list.begin(); iter != list.end(); ++iter)
        {
            data[i] = *iter;
            i++;
        }
    }

    /**
     * @brief Constructor
     * @param list The list of values in the CGMatrix (row-major order)
     */
    CGMatrix(const std::initializer_list<std::initializer_list<float>>& list)
    {
        CG_ERROR_COND_EXIT(list.size() != N, -1, CGSTR("The size of the initializer list is not equal to the size of the CGMatrix"));
        int i = 0;
        int j = 0;
        for (auto iter = list.begin(); iter != list.end(); ++iter)
        {
            CG_ERROR_COND_EXIT(iter->size() != N, -1, CGSTR("The size of the initializer list is not equal to the size of the CGMatrix"));
            j = 0;
            for (auto iter2 = iter->begin(); iter2 != iter->end(); ++iter2)
            {
                data_m[i][j] = *iter2;
                ++j;
            }
            ++i;
        }
    }

    /**
     * @brief The identity CGMatrix
     * 
     * @return const CGMatrix<N> The identity CGMatrix
     */
    inline static const CGMatrix<N>& Identity() noexcept
    {
        if (is_identity_initialized)
            return IDENTITY;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; i < N; ++j)
            {
                IDENTITY[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
        is_identity_initialized = true;
        return IDENTITY;
    }

    /**
     * @brief Constructor
     * @param list The list of values in the CGMatrix (row-major order)
     */
    constexpr float* operator[](int i) noexcept
    {
        return data_m[i];
    }

    /**
     * @brief Constructor
     * @param list The list of values in the CGMatrix (row-major order)
     */
    constexpr const float* operator[](int i) const noexcept
    {
        return data_m[i];
    }

    /**
     * @brief Get the array of this CGMatrix in column-major order
     * 
     * @return float* The CGMatrix in column-major order
     */
    inline const float* GetCColumnMajorMatrix() const noexcept
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                CGMatrix_buffer[i * N + j] = data_m[j][i];
            }
        }
        return CGMatrix_buffer;
    }

    /**
     * @brief Get the CGMatrix in column-major order
     * 
     * @return const CGMatrix<N> The CGMatrix in column-major order
     */
    inline CGMatrix<N> GetColumnMajorMatrix() const noexcept
    {
        return CGMatrix<N>(GetCColumnMajorMatrix(), SIZE);
    }

    inline CGMatrix<N> operator*(const CGMatrix<N>& other) const noexcept
    {
        CGMatrix<N> result;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; i < N; ++j)
            {
                result[i][j] = 0;
                for (int k = 0; k < N; ++k)
                {
                    result[i][j] += data_m[i][k] * other[k][j];
                }
            }
        }
        return result;
    }

    inline CGMatrix<N>& operator*=(const CGMatrix<N>& other) noexcept
    {
        *this = *this * other;
        return *this;
    }

    inline CGMatrix<N> operator+(const CGMatrix<N>& other) noexcept
    {
        CGMatrix<N> result;
        for (int i = 0; i < SIZE; ++i)
        {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    inline CGMatrix<N>& operator+=(const CGMatrix<N>& other) noexcept
    {
        for (int i = 0; i < SIZE; ++i)
        {
            data[i] += other.data[i];
        }
        return *this;
    }
};


namespace CGMat3
{
    using CGMat3 = CGMatrix<3>;

    /**
     * @brief Get the position matrix
     * 
     * @param position The position to get the matrix out from
     * @return CGMat3 The position matrix
     */
    inline static CGMat3 GetPositionMatrix(const CGVector2& position) noexcept
    {
        return CGMat3({1.0f, 0.0f, position.x,
                    0.0f, 1.0f, position.y,
                    0.0f, 0.0f, 1.0f});
    }

    /**
     * @brief Get the rotation matrix
     * 
     * @param rotation The rotation to get the matrix out from
     * @return CGMat3 The rotation matrix
     */
    inline static CGMat3 GetRotationMatrix(float rotation) noexcept
    {
        float sin_theta = sin(rotation);
        float cos_theta = cos(rotation);
        return CGMat3({cos_theta, -sin_theta, 0.0f,
                    sin_theta, cos_theta, 0.0f,
                    0.0f, 0.0f, 1.0f});
    }

    /**
     * @brief Get the scale matrix
     * 
     * @param scale The scale to get the matrix out from
     * @return CGMat3 The scale matrix
     */
    inline static CGMat3 GetScaleMatrix(const CGVector2& scale) noexcept
    {
        return CGMat3({scale.x, 0.0f, 0.0f,
                    0.0f, scale.y, 0.0f,
                    0.0f, 0.0f, 1.0f});
    }
}

    
inline static CGVector2 operator*(const CGMat3::CGMat3& m, const CGVector2& v) noexcept
{
    return (CGVector2){.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2],
                    .y = m[1][0] * v.x + m[1][1] * v.y + m[1][2]};
}