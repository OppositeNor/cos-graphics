#pragma once
/**
 * @brief Component is a base class for all the items that exists in the game world.
 * 
 */
#include "cos_graphics/graphics.h"
class CGComponent
{
protected:

    /**
     * @brief Transform component that contains the transformation of components.
     * 
     */
    struct CGTransform
    {
        /**
         * @brief The position of the component
         */
        CGVector2 position;
        /**
         * @brief The rotation of the component 
         */
        float rotation;
        /**
         * @brief The scale of the component 
         */
        CGVector2 scale;

        /**
         * @brief Construct a new Transform object
         * 
         * @param p_position The position of the component
         * @param p_rotation The rotation of the component
         * @param p_scale The scale of the component 
         */
        CGTransform(const CGVector2& p_position = {0.0f, 0.0f}, float p_rotation = 0.0f, const CGVector2& p_scale = {1.0f, 1.0f});
        virtual ~CGTransform() {}
    }transform;

    /**
     * @brief Called once when the component is created.
     */
    virtual void Ready() {}

    /**
     * @brief Called every frame after the component is created.
     * 
     * @param p_delta_time the time in seconds since the last frame.
     */
    virtual void Update(double p_delta_time) {}

public:

    /**
     * @brief Construct a new CGComponent object
     * 
     */
    CGComponent();

    virtual ~CGComponent();

    /**
     * @brief Called every frame by the engine.
     * 
     * @param p_delta_time The difference in time between frames
     */
    virtual void Tick(double p_delta_time);
};