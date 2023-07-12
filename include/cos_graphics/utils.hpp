#pragma once
#include <initializer_list>
#include "cos_graphics/graphics.h"

class CGUtils
{
    CGUtils() {}
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
    static std::vector<CGVisualImage*>&& CGCreateVisualImageList(const std::initializer_list<std::string>& p_image_rks)
    {
        std::vector<CGVisualImage*> list;
        for (auto iter : p_image_rks)
        {
            list.insert(list.end(), CGCreateVisualImage(iter.c_str(), CGGame::GetInstance()->GetGameWindow()));
        }
        return std::move(list);
    }
    
};
