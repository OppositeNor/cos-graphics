#pragma once

/**
 * @brief Interface for objects that can be enclosed by a boarder.
 * @note A boarder is a unrotated rectangle that enclosed the whole component. The whole
 * component also includes the children of the component.
 */
class CGIRectBoarder
{
public:
    /**
     * @brief Get the width of the boarder.
     * 
     * @return float The width of the boarder.
     */
    virtual float GetBoarderWidth() const = 0;
    /**
     * @brief Get the height of the boarder.
     * 
     * @return float The height of the boarder.
     */
    virtual float GetBoarderHeight() const = 0;

    /**
     * @brief Get the y coordinate value of the top of the boarder.
     * 
     * @return float The y coordinate value of the top of the boarder.
     */
    virtual float GetBoarderTopY() const = 0;
    /**
     * @brief Get the y coordinate value of the bottom of the boarder.
     * 
     * @return float The y coordinate value of the bottom of the boarder.
     */
    virtual float GetBoarderBottomY() const = 0;

    /**
     * @brief Get the x coordinate value of the left of the boarder.
     * 
     * @return float The x coordinate value of the left of the boarder.
     */
    virtual float GetBoarderLeftX() const = 0;

    /**
     * @brief Get the x coordinate value of the right of the boarder.
     * 
     * @return float The x coordinate value of the right of the boarder.
     */
    virtual float GetBoarderRightX() const = 0;
};