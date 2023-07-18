#pragma once
/**
 * @brief Component is a base class for all the items that exists in the game world.
 * 
 */
#include "cos_graphics/graphics.h"
#include <vector>

class CGComponent
{

    /**
     * @brief The parent of the component. The child will follow the parent's transformation.
     * If the parent is NULL, the component will be in world space. The children will also follow
     * the parent's life cycle.
     * 
     * @todo Implement parent-child relationship
     */
    CGComponent* parent = nullptr;

    /**
     * @brief The children of the component. The children will follow the component's transformation.
     * When the component is destroyed, the children will also be destroyed.
     * 
     * @todo Implement parent-child relationship
     */
    std::vector<CGComponent*> children;

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
    const CGTransform& GetTransform() const noexcept;

    /**
     * @brief Get the reference of the Transform object
     * 
     * @return CGComponent::CGTransform& The reference of the transform object
     */
    CGTransform& GetTransform() noexcept;

    /**
     * @brief Get the global transform of the component
     * 
     * @return CGTransform The global transform of the component
    */
    CGTransform GetGlobalTransform() const;

    /**
     * @brief Set the Depth object
     * 
     * @param p_depth The new depth to be set to
     */
    void SetDepth(float p_depth);

    /**
     * @brief Add a child to the component.
     * 
     * @param p_child The child to be added.
     */
    void AddChild(CGComponent* p_child);

    /**
     * @brief Remove a child from the component.
     * 
     * @param p_child The child to be removed.
     */
    void RemoveChild(CGComponent* p_child);

    /**
     * @brief Set the Parent object
     * 
     * @param p_parent The parent to be set to
     */
    void SetParent(CGComponent* p_parent);

    /**
     * @brief Detach a child from the component.
     * 
     * @param p_child The child to be detached.
     */
    void DetachChild(CGComponent* p_child);
};