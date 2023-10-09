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
    static float matrix_buffer[SIZE];
    inline static bool is_identity_initialized = false;

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
                matrix_buffer[i * N + j] = data_m[j][i];
            }
        }
        return matrix_buffer;
    }

    inline const float* GetCMatrix() const noexcept
    {
        for (int i = 0; i < SIZE; ++i)
        {
            matrix_buffer[i] = data[i];
        }
        return matrix_buffer;
    }

    inline CGMatrix<N> operator*(const CGMatrix<N>& p_other) const noexcept
    {
        CGMatrix<N> result;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                result[i][j] = 0;
                for (int k = 0; k < N; ++k)
                {
                    result[i][j] += data_m[i][k] * p_other[k][j];
                }
            }
        }
        return result;
    }

    inline CGMatrix<N> operator*(float p_scalar) const noexcept
    {
        CGMatrix<N> result;
        for (int i = 0; i < SIZE; ++i)
        {
            result.data[i] = data[i] * p_scalar;
        }
        return result;
    }

    inline CGMatrix<N>& operator*=(float p_scalar) noexcept
    {
        for (int i = 0; i < SIZE; ++i)
        {
            data[i] *= p_scalar;
        }
        return *this;
    }

    inline CGMatrix<N>& operator*=(const CGMatrix<N>& p_other) noexcept
    {
        *this = *this * p_other;
        return *this;
    }

    inline CGMatrix<N> operator+(const CGMatrix<N>& p_other) noexcept
    {
        CGMatrix<N> result;
        for (int i = 0; i < SIZE; ++i)
        {
            result.data[i] = data[i] + p_other.data[i];
        }
        return result;
    }

    inline CGMatrix<N>& operator+=(const CGMatrix<N>& p_other) noexcept
    {
        for (int i = 0; i < SIZE; ++i)
        {
            data[i] += p_other.data[i];
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
    CGMat3 GetPositionMatrix(const CGVector2& p_position) noexcept;

    /**
     * @brief Get the rotation matrix
     * 
     * @param rotation The rotation to get the matrix out from
     * @return CGMat3 The rotation matrix
     */
    CGMat3 GetRotationMatrix(float p_rotation) noexcept;

    /**
     * @brief Get the inverse rotation matrix
     * 
     * @param p_rotation The rotation to get the matrix out from
     * @return CGMat3 The inverse rotation matrix
     */
    CGMat3 GetInvRoationMatrix(float p_rotation) noexcept;

    /**
     * @brief Get the scale matrix
     * 
     * @param scale The scale to get the matrix out from
     * @return CGMat3 The scale matrix
     */
    CGMat3 GetScaleMatrix(const CGVector2& p_scale) noexcept;
}

    
inline static CGVector2 operator*(const CGMat3::CGMat3& p_m, const CGVector2& p_v) noexcept
{
    return (CGVector2){p_m[0][0] * p_v.x + p_m[0][1] * p_v.y + p_m[0][2],
                        p_m[1][0] * p_v.x + p_m[1][1] * p_v.y + p_m[1][2]};
}