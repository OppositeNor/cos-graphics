#pragma once
#include "cos_graphics/component/visual_component.h"

class CGTextComponent : public CGVisualComponent
{
    CG_COMPONENT(CGTextComponent)
private:
    CGString text;
    CGString text_rk;
    CGString font_rk;
    CGTextProperty text_property;
    CGVisualImage* text_image = nullptr;

    void UpdateTextImage();
protected:
    virtual void Draw(float p_delta) override;

public:
    /**
     * @brief Construct a new CGTextComponent object with the default font.
     * 
     * @param p_text_rk The resource key of the text.
     * @param p_text_property The property of the text.
     */
    CGTextComponent(const CGString& p_text_rk, const CGTextProperty& p_text_property);

    /**
     * @brief Construct a new CGTextComponent object.
     * 
     * @param p_text_rk The resource key of the text.
     * @param p_font_rk The resource key of the font.
     * @param p_text_property The property of the text.
     */
    CGTextComponent(const CGString& p_text_rk, const CGString& p_font_rk, const CGTextProperty& p_text_property);
    
    /**
     * @brief Copy constructor.
     * 
     * @param other The other CGTextComponent object.
     */
    CGTextComponent(const CGTextComponent& other);

    /**
     * @brief Move constructor.
     * 
     * @param other The other CGTextComponent object.
     */
    CGTextComponent(CGTextComponent&& other);

    /**
     * @brief Set the text that the component will draw.
     * 
     * @param p_text_rk The resource key of the text to be set to.
     */
    void SetText(const CGString& p_text_rk);

    /**
     * @brief Get the text that the component will draw.
     * 
     * @return const CGString 
     */
    const CGString& GetText() const noexcept;

    /**
     * @brief Set the font of the text.
     * 
     * @param p_font_rk The resource key of the font to be set to.
     */
    void SetFont(const CGString& p_font_rk);

    /**
     * @brief Get the property of the text.
     * 
     * @return const CGTextProperty& The property of the text.
     */
    const CGTextProperty& GetTextProperty() const noexcept;
    
    /**
     * @brief Set the property of the text.
     * 
     * @param p_property The property that the text will be set to.
     */
    void SetTextProperty(const CGTextProperty& p_property);

    /**
     * @brief Destroy the CGTextComponent object.
     */
    virtual ~CGTextComponent() override;
};