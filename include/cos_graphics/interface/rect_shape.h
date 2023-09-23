#pragma once

class CGIRectShape
{
public:
    /**
     * @brief Get the width of the visual component.
     * 
     * @return float The width of the visual component.
     */
    virtual float GetWidth() const = 0;
    /**
     * @brief Get the height of the visual component
     * 
     * @return float The height of the visual component
     */
    virtual float GetHeight() const = 0;
};