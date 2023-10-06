#pragma once
/**
 * @brief Component is a base class for all the items that exists in the game world.
 * 
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
     * @brief Should the global transform matrix and the inv global transform matrix been updated.
     */
    bool should_update_matrix = true;

    /**
     * @brief The global transform matrix.
     */
    CGMat3::CGMat3 global_transform_matrix;

    /**
     * @brief The inverse of the global transform matrix.
     */
    CGMat3::CGMat3 global_inv_transform_matrix;

    /**
     * @brief Setup the should_update_matrix flag.
     */
    void ShouldUpdateMatrix();

    /**
     * @brief The label of the component. The label is used to identify the component.
     */
    CGString label = CGSTR("");
protected:
    /**
     * @brief Is the component visible in the game.
     */
    bool visible = true;

public:
    /**
     * @brief The type of the component. The CG_COMPONENT_TYPE macro will automatically override this function.
     * 
     * @return CGString 
     */
    inline virtual CGString GetComponentType() const noexcept {return CGSTR("CGComponent");}

    /**
     * @brief Get the label of the component.
     */
    inline const CGString GetLabel() const noexcept { return GetComponentType() + CGSTR(": ") + label; }
    
    /**
     * @brief Get the label without the type of the component.
     */
    inline const CGString& GetLabelRaw() const noexcept { return label; }

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
     * @note This function is different from @ref IsVisible(). IsVisible will return true
     * if the component is globaly visible, and this function will only return if the
     * component is locally visible. Such as when you call SetVisible and set the visible
     * to true for the component, it can still be invisible if the parent is invisible in
     * the game. In this case IsVisible() will return false and GetVisible() will return 
     * true.
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
     * @return true The component is visible in the game
     * @return false The component is not visible in the game
     */
    bool IsVisible() const noexcept;

    /**
     * @brief Called when the component is added to the game.
     */
    void OnEnter();

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
    CGMat3::CGMat3 GetGlobalInvTransformMatrix() const noexcept;

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
     * @brief Move constructor.
     * @param other The other CGComponent object.
     */
    CGComponent(CGComponent&& other) noexcept;

    virtual ~CGComponent();

    /**
     * @brief Get the width of the boarder.
     * @note rotation is not considered.
     * @return float The width of the boarder.
     */
    inline virtual float GetBoarderWidth() noexcept override { return 0.0f; };
    /**
     * @brief Get the height of the boarder.
     * @note rotation is not considered.
     * @return float The height of the boarder.
     */
    inline virtual float GetBoarderHeight() noexcept override { return 0.0f; };

    /**
     * @brief Get the y coordinate value of the top of the boarder.
     * @note rotation is not considered.
     * @return float The y coordinate value of the top of the boarder.
     */
    virtual float GetBoarderTopY() noexcept override;
    /**
     * @brief Get the y coordinate value of the bottom of the boarder.
     * @note rotation is not considered.
     * @return float The y coordinate value of the bottom of the boarder.
     */
    virtual float GetBoarderBottomY() noexcept override;

    /**
     * @brief Get the x coordinate value of the left of the boarder.
     * @note rotation is not considered.
     * @return float The x coordinate value of the left of the boarder.
     */
    virtual float GetBoarderLeftX() noexcept override;

    /**
     * @brief Get the x coordinate value of the right of the boarder.
     * @note rotation is not considered.
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
     * @brief Get the y coordinate value of the top of the shape.
     * 
     * @return float The y coordinate value of the top of the shape.
     */
    inline virtual float GetTopY() const noexcept override { return 0.0f; }
    /**
     * @brief Get the y coordinate value of the bottom of the shape.
     * 
     * @return float The y coordinate value of the bottom of the shape.
     */
    inline virtual float GetBottomY() const noexcept override { return 0.0f; }

    /**
     * @brief Get the x coordinate value of the left of the shape.
     * 
     * @return float The x coordinate value of the left of the shape.
     */
    inline virtual float GetLeftX() const noexcept override { return 0.0f; }

    /**
     * @brief Get the x coordinate value of the right of the shape.
     * 
     * @return float The x coordinate value of the right of the shape.
     */
    inline virtual float GetRightX() const noexcept override { return 0.0f; }
    
    /**
     * @brief Allign the top of the image to the top of the window.
     * 
     * @param p_offset The y offset of the image after allignment.
     */
    void AllignTop(float p_offset = 0.0f);

    /**
     * @brief Allign the bottom of the image to the bottom of the window.
     * 
     * @param p_offset The y offset of the image after allignment.
     */
    void AllignBottom(float p_offset = 0.0f);

    /**
     * @brief Allign the left of the image to the left of the window.
     * 
     * @param p_offset The x offset of the image after allignment.
     */
    void AllignLeft(float p_offset = 0.0f);

    /**
     * @brief Allign the right of the image to the right of the window.
     * 
     * @param p_offset The x offset of the image after allignment.
     */
    void AllignRight(float p_offset = 0.0f);

    /**
     * @brief Allign the bottom of the image to the top of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after allignment.
     */
    void AllignBottomToTop(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the top of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after allignment.
     */
    void AllignTopToTop(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the top of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after allignment.
     */
    void AllignTopToBottom(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the bottom of the image to the bottom of a target component.
     * 
     * @param target The target component.
     * @param p_offset The y offset of the image after allignment.
     */
    void AlignBottomToBottom(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the left of the image to the right of a target component.
     * 
     * @param target The target component.
     * @param p_offset The x offset of the image after allignment.
     */
    void AllignLeftToRight(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the left of the image to the left of the target component.
     * 
     * @param target The target component
     * @param p_offset The x offset of the image after allignment.
     */
    void AllignRightToRight(CGIRectBoarder* target, float p_offset = 0.0f);

    /**
     * @brief Allign the right of the component to the left of the target component.
     * 
     * @param target The target component
     * @param p_offset The x offset of the image after allignment.
     */
    void AllignRightToLeft(CGIRectBoarder* target, float p_offset = 0.0f);
    
    /**
     * @brief Allign the left of the 
     * 
     * @param target 
     * @param p_offset 
     */
    void AllignLeftToLeft(CGIRectBoarder* target, float p_offset = 0.0f);
    
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