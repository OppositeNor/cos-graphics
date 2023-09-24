#pragma once
/**
 * @brief Interface for rectangluar shapes.
 * 
 */
class CGIRectShape
{
public:
    /**
     * @brief Get the width of the shape.
     * 
     * @return float The width of the shape.
     */
    virtual float GetWidth() const = 0;
    /**
     * @brief Get the height of the shape.
     * 
     * @return float The height of the shape.
     */
    virtual float GetHeight() const = 0;

    /**
     * @brief Get the y coordinate value of the top of the shape.
     * 
     * @return float The y coordinate value of the top of the shape.
     */
    virtual float GetTopY() const = 0;
    /**
     * @brief Get the y coordinate value of the bottom of the shape.
     * 
     * @return float The y coordinate value of the bottom of the shape.
     */
    virtual float GetBottomY() const = 0;

    /**
     * @brief Get the x coordinate value of the left of the shape.
     * 
     * @return float The x coordinate value of the left of the shape.
     */
    virtual float GetLeftX() const = 0;

    /**
     * @brief Get the x coordinate value of the right of the shape.
     * 
     * @return float The x coordinate value of the right of the shape.
     */
    virtual float GetRightX() const = 0;

};