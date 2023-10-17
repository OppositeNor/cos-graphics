#pragma once
/**
 * @file component.h
 * @author OppositeNor
 * @brief The base class for all the components in the game.
 */
#include "cos_graphics/graphics.h"
#include <vector>
#include <string>
#include "cos_graphics/interface/rect_boarder.h"
#include "cos_graphics/interface/rect_shape.h"
#include "cos_graphics/math/matrix.hpp"


/**
 * @brief Prefix of component. The region under this macro will be set to private.
 * Everytime you create a component you should put this macro inside it. Use 
 * @code CG_COMPONENT(YourComponentName).
 * @endcode
 *  
 */
#define CG_COMPONENT(type) \
    public:                     \
    inline virtual CGString GetComponentType() const noexcept override { return CGSTR(#type); } \
    private:                

/**
 * @brief The base class for all the components in the game.
 * 
 * @details 
 * A Component is any item that exists in the game. Every component has a transform
 * which contains the position, rotation and scale of the component. The component
 * can be attached to a parent component, and the children component will inherit
 * the parent's transformation and the visibility of the parent.
 * 
 * When defining your own component, you have to put CG_COMPONENT(YourClassName) at
 * the beginning of the class. The region under CG_COMPONENT will be set to private.
 * 
 * This is an example of a component class:
 * 
 * @code
 * class YourComponentName : public CGComponent
 * {
 *     CG_COMPONENT(YourComponentName)
 * private:
 * ... (your properties and functions)
 * }
 * @endcode
 */
class CGComponent :
    public CGIRectBoarder
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

    /**
     * @brief Should the global transform matrix.
     */
    bool should_update_global_matrix = true;

    /**
     * @brief Should the inverse of the global transform matrix be updated
     */
    bool should_update_inv_global_matrix = true;

    /**
     * @brief The global transform matrix.
     */
    CGMat3::CGMat3 global_transform_matrix;

    /**
     * @brief The inverse of the global transform matrix.
     */
    CGMat3::CGMat3 inv_global_transform_matrix;

    /**
     * @brief Setup the should_update_matrix flag.
     */
    void ShouldUpdateMatrix();

    /**
     * @brief The label of the component. The label is used to identify the component.
     */
    CGString label = CGSTR("");

    /**
     * @brief Is the component activated.
     */
    bool activated = false;

public:

    /**
     * @brief The type of the component. The CG_COMPONENT_TYPE macro will automatically override this function.
     * 
     * @return CGString 
     */
    inline virtual CGString GetComponentType() const noexcept {return CGSTR("CGComponent");}

    /**
     * @brief Called once when the component is activated.
     */
    virtual void Ready() {}

    /**
     * @brief Called every frame after the component is activated.
     * 
     * @param p_delta_time the time in seconds since the last frame.
     */
    virtual void Update(double p_delta_time) {}
    

    /**
     * @brief Called when a keyboard input is received.
     * 
     * @param p_key The key that is receiving the action.
     * @param p_action The action of the key. This should be one of CG_PRESS, CG_RELEASE, CG_REPEAT.
     */
    virtual void KeyInput(int p_key, int p_action) {}

    /**
     * @brief Move constructor.
     * @param other The other CGComponent object.
     */
    CGComponent(CGComponent&& other) noexcept;

    virtual ~CGComponent();

    /**
     * @brief Get the width of the boarder.
     * @details The global width of the boarder is used for alignment. It can calculate the whole component's 
     * demensions (including the children). You can override this function if you want to use different method
     * in alignment.
     * @return float The width of the boarder.
     */
    inline virtual float GetBoarderWidth() noexcept override { return 0.0f; };
    /**
     * @brief Get the height of the boarder.
     * @details The global height of the boarder is used for alignment. It can calculate the whole component's 
     * demensions (including the children). You can override this function if you want to use different method
     * in alignment.
     * @return float The height of the boarder.
     */
    inline virtual float GetBoarderHeight() noexcept override { return 0.0f; };

    /**
     * @brief Get the y coordinate value of the top of the boarder.
     * @details The global y coordinate value of the top of the boarder is used for alignment. It can calculate 
     * the whole component's demensions (including the children). You can override this function if you want to 
     * use different method in alignment.
     * @return float The y coordinate value of the top of the boarder.
     */
    virtual float GetBoarderTopY() noexcept override;
    /**
     * @brief Get the y coordinate value of the bottom of the boarder.
     * @details The global y coordinate value of the bottom of the boarder is used for alignment. It can calculate 
     * the whole component's demensions (including the children). You can override this function if you want to 
     * use different method in alignment.
     * @return float The y coordinate value of the bottom of the boarder.
     */
    virtual float GetBoarderBottomY() noexcept override;

    /**
     * @brief Get the x coordinate value of the left of the boarder.
     * @details The global x coordinate value of the left of the boarder is used for alignment. It can calculate 
     * the whole component's demensions (including the children). You can override this function if you want to 
     * use different method in alignment.
     * @return float The x coordinate value of the left of the boarder.
     */
    virtual float GetBoarderLeftX() noexcept override;

    /**
     * @brief Get the x coordinate value of the right of the boarder.
     * @details The global x coordinate value of the right of the boarder is used for alignment. It can calculate 
     * the whole component's demensions (including the children). You can override this function if you want to 
     * use different method in alignment.
     * @return float The x coordinate value of the right of the boarder.
     */
    virtual float GetBoarderRightX() noexcept override;

    /**
     * @brief Called every frame by the engine.
     * 
     * @param p_delta_time The difference in time between frames.
     */
    virtual void Tick(double p_delta_time);

    /**
     * @brief Get the width of the shape.
     * 
     * @return float The width of the shape.
     */
    inline virtual float GetWidth() const noexcept override { return 0.0f; };
    /**
     * @brief Get the height of the shape.
     * 
     * @return float The height of the shape.
     */
    inline virtual float GetHeight() const noexcept override { return 0.0f; };

    /**
     * @brief Get the local y coordinate value of the top of the shape.
     * @details This function only get the local coordinate value of theshape. It does not 
     * take the children and parent's transformation into account. If you want to get the 
     * global y coordinate value of the top of the shape, use @ref GetBoarderTopY.
     * @return float The y coordinate value of the top of the shape.
     */
    inline virtual float GetTopY() const noexcept override { return 0.0f; }
    /**
     * @brief Get the y coordinate value of the bottom of the shape.
     * @details This function only get the local coordinate value of theshape. It does not 
     * take the children and parent's transformation into account. If you want to get the 
     * global y coordinate value of the bottom of the shape, use @ref GetBoarderBottomY.
     * @return float The y coordinate value of the bottom of the shape.
     */
    inline virtual float GetBottomY() const noexcept override { return 0.0f; }

    /**
     * @brief Get the x coordinate value of the left of the shape.
     * @details This function only get the local coordinate value of theshape. It does not 
     * take the children and parent's transformation into account. If you want to get the 
     * global x coordinate value of the left of the shape, use @ref GetBoarderLeftX.
     * @return float The x coordinate value of the left of the shape.
     */
    inline virtual float GetLeftX() const noexcept override { return 0.0f; }

    /**
     * @brief Get the x coordinate value of the right of the shape.
     * @details This function only get the local coordinate value of theshape. It does not 
     * take the children and parent's transformation into account. If you want to get the 
     * global x coordinate value of the right of the shape, use @ref GetBoarderRightX.
     * @return float The x coordinate value of the right of the shape.
     */
    inline virtual float GetRightX() const noexcept override { return 0.0f; }

protected:
    /**
     * @brief Is the component visible in the game.
     * @details If this is set to false, this component and all of its children will not be drawn.
     */
    bool visible = true;

    /**
     * @brief Is the component queued to be freed.
     */
    bool is_queue_freed = false;
public:

    /**
     * @brief Get the label of the component.
     */
    inline const CGString GetLabel() const noexcept { return GetComponentType() + CGSTR(": ") + label; }
    
    /**
     * @brief Get the label without the type of the component.
     */
    inline const CGString& GetLabelRaw() const noexcept { return label; }

    /**
     * @brief Is the component queued to free.
     * 
     * @note When freeing a component, the children of the component will not be freed.
     * So you have to free the children of the component manually.
     * 
     * @return true The component is queued to free.
     * @return false The component is not queued to free.
     */
    inline const bool IsQueueFreed() const noexcept { return is_queue_freed; }

    /**
     * @brief Get the label of the component.
     */
    CGString& GetLabel() noexcept { return label; }

    /**
     * @brief Transform component that contains the transformation of components.
     * 
     */
    struct CGTransform
    {
        /**
         * @brief The position of the component.
         */
        CGVector2 position;
        /**
         * @brief The rotation of the component.
         */
        float rotation;
        /**
         * @brief The scale of the component.
         */
        CGVector2 scale;

        /**
         * @brief The depth of the component. The higher the depth, the more in behind the component is.
         */
        float depth = 0.0f;

        /**
         * @brief Construct a new Transform object.
         * 
         * @param p_position The position of the component.
         * @param p_rotation The rotation of the component.
         * @param p_scale The scale of the component.
         */
        CGTransform(const CGVector2& p_position = {0.0f, 0.0f}, float p_rotation = 0.0f, const CGVector2& p_scale = {1.0f, 1.0f});

        /**
         * @brief Get the transform matrix of the component.
         * 
         * @return CGMat3 The transform matrix of the component.
         */
        CGMat3::CGMat3 GetTransformMatrix() const noexcept;

        /**
         * @brief Get the inverse transform matrix of the component.
         */
        CGMat3::CGMat3 GetInvTransformMatrix() const noexcept;

        virtual ~CGTransform() {}
    };
    

    /**
     * @brief Set if the component is visible in the game
     * 
     * @param p_visible The new value of visible
     */
    void SetVisible(bool p_visible) noexcept;

    /**
     * @brief Get if the component is visible.
     * 
     * @note This function is different from @ref IsVisible(). IsVisible will return true
     * if the component is globaly visible, and this function will only return if the
     * component is locally visible. Such as when you call SetVisible and set the visible
     * to true for the component, it can still be invisible if the parent is invisible in
     * the game. In this case IsVisible() will return false and GetVisible() will return 
     * true.
     * 
     * @return true The component is locally visible
     * @return false The component is locally invisible
     */
    bool GetVisible() const noexcept;

    /**
     * @brief Is the component visible in the game
     * 
     * @note This function is different from @ref GetVisible(). IsVisible will return true
     * if the component is globaly visible, and this function will only return if the
     * component is locally visible. Such as when you call SetVisible and set the visible
     * to true for the component, it can still be invisible if the parent is invisible in
     * the game. In this case IsVisible() will return false and GetVisible() will return 
     * true.
     * 
     * @return true The component is visible in the game
     * @return false The component is not visible in the game
     */
    bool IsVisible() const noexcept;

    /**
     * @brief Called when the component is added to the game.
     */
    void OnEnter();

    /**
     * @brief Queue the component to be freed at the end of the frame.
     * @details You can check a component is queued to be freed by calling @ref IsQueueFreed.
     */
    void QueueFree();
private:

    /**
     * @brief The transform of the component.
     */
    CGTransform transform;

protected:

    /**
     * @brief Get the inverse global transform matrix of the component.
     * 
     * @return CGMat3::CGMat3 The inverse global transform matrix of the component.
     */
    CGMat3::CGMat3 GetInvGlobalTransformMatrix() noexcept;

public:
    /**
     * @brief Get the global transform matrix of the component.
     * 
     * @return CGMat3::CGMat3 The global transform matrix of the component.
     */
    CGMat3::CGMat3 GetGlobalTransformMatrix() noexcept;

    /**
     * @brief Construct a new CGComponent object.
     */
    CGComponent();

    /**
     * @brief Copy constructor.
     * @note The parent and children will not be copied.
     * @param other The other CGComponent object.
     */
    CGComponent(const CGComponent& other);
    
    /**
     * @brief Align the top of the image to the top of the window.
     * 
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignTop(float p_offset = 0.0f);

    /**
     * @brief Align the bottom of the image to the bottom of the window.
     * 
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignBottom(float p_offset = 0.0f);

    /**
     * @brief Align the left of the image to the left of the window.
     * 
     * @param p_offset The x offset of the image after alignment.
     */
    void AlignLeft(float p_offset = 0.0f);

    /**
     * @brief Align the right of the image to the right of the window.
     * 
     * @param p_offset The x offset of the image after alignment.
     */
    void AlignRight(float p_offset = 0.0f);

    /**
     * @brief Align the bottom of the image to the top of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignBottomToTop(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the top of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignTopToTop(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the top of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignTopToBottom(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the bottom of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after alignment.
     */
    void AlignBottomToBottom(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the left of the image to the right of a target component.
     * 
     * @param target The target component.
     * @param p_offset The x offset of the image after alignment.
     */
    void AlignLeftToRight(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the left of the image to the left of the target component.
     * 
     * @param target The target component
     * @param p_offset The x offset of the image after alignment.
     */
    void AlignRightToRight(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Align the right of the component to the left of the target component.
     * 
     * @param target The target component
     * @param p_offset The x offset of the image after alignment.
     */
    void AlignRightToLeft(CGIRectBoarder* target, float p_offset = 0.0f);
    
    /**
     * @brief Align the left of the 
     * 
     * @param target 
     * @param p_offset 
     */
    void AlignLeftToLeft(CGIRectBoarder* target, float p_offset = 0.0f);
    
    /**
     * @brief Get the Transform object.
     * 
     * @return CGTransform& The transform of the component.
     */
    inline const CGTransform& GetTransform() const noexcept { return transform; }

    /**
     * @brief Get the reference of the Transform object.
     * 
     * @return CGComponent::CGTransform& The reference of the transform object.
     */
    CGTransform& GetTransform() noexcept;
    
    /**
     * @brief Convert a global position to a relative position.
     * 
     * @param global_position The global position to be converted.
     * @return CGVector2 The relative position of the component.
     */
    CGVector2 ToRelativePosition(const CGVector2& global_position) const;

    /**
     * @brief Convert a relative position to a global position.
     * 
     * @param relative_position The relative position to be converted.
     * @return CGVector2 The global position of the component.
     */
    CGVector2 ToGlobalPosition(const CGVector2& relative_position) const;
    
    /**
     * @brief Get the global position of the component.
     * 
     * @return CGVector2 The global position of the component.
     */
    CGVector2 GetGlobalPosition() const;

    /**
     * @brief Set the global position of the component.
     * 
     */
    void SetGlobalPosition(const CGVector2& global_position);

    /**
     * @brief Set the Global Position object
     * 
     * @return CGVector2 The global position of the component.
     */
    CGVector2 SetGlobalPosition() const;

    /**
     * @brief Set the Depth object.
     * 
     * @param p_depth The new depth to be set to.
     */
    void SetDepth(float p_depth);

    /**
     * @brief Add a child to the component.
     * 
     * @param p_child The child to be added.
     */
    void AddChild(CGComponent* p_child);

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

    /**
     * @brief Get the Parent object
     * 
     * @return CGComponent* The parent of the component.
     */
    CGComponent* GetParent() const noexcept;

    /**
     * @brief Get children
     * 
     * @return const std::vector<CGComponent*>& The children of the component.
     */
    const std::vector<CGComponent*>& GetChildren() const noexcept;
};