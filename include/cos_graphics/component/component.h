#pragma once
/**
 * @brief Component is a base class for all the items that exists in the game world.
 * 
 */
#include "cos_graphics/graphics.h"
class CGComponent
{
public:
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
         * @brief The depth of the component. The higher the depth, the more in behind the component is.
         */
        float depth = 0.0f;

        /**
         * @brief Construct a new Transform object
         * 
         * @param p_position The position of the component
         * @param p_rotation The rotation of the component
         * @param p_scale The scale of the component 
         */
        CGTransform(const CGVector2& p_position = {0.0f, 0.0f}, float p_rotation = 0.0f, const CGVector2& p_scale = {1.0f, 1.0f});
        virtual ~CGTransform() {}
    };
protected:

    /**
     * @brief Has the component's transform property been set
     */
    bool has_transform_set = false;

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

    CGTransform transform;

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
    
    /**
     * @brief Get the Transform object
     * 
     * @return CGTransform& The transform of the component
     */
    const CGTransform& GetTransform() const;

    /**
     * @brief Set the Transform object
     * 
     * @param p_transform The new transform of the component
     */
    void SetTransform(const CGTransform& p_transform);

    /**
     * @brief Set the Position of the component
     * 
     * @param p_position The new position to be set to
     */
    void SetPosition(const CGVector2& p_position);

    /**
     * @brief Set the Rotation of the component
     * 
     * @param p_rotation The new rotation to be set to
     */
    void SetRotation(float p_rotation);

    /**
     * @brief Set the Scale of the component
     * 
     * @param p_scale The new scale to be set to
     */
    void SetScale(const CGVector2& p_scale);

    /**
     * @brief Set the Depth object
     * 
     * @param p_depth The new depth to be set to
     */
    void SetDepth(float p_depth);
};