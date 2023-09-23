#pragma once
#include "cos_graphics/component/visual_component.h"

/**
 * @brief A text 
 * 
 */
class CGText : public CGVisualComponent
{
    CG_COMPONENT(CGText)
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
     * @brief Construct a new CGText object with the default font.
     * 
     * @param p_text_rk The resource key of the text.
     * @param p_text_property The property of the text.
     */
    CGText(const CGString& p_text_rk, const CGTextProperty& p_text_property);

    /**
     * @brief Construct a new CGText object.
     * 
     * @param p_text_rk The resource key of the text.
     * @param p_font_rk The resource key of the font.
     * @param p_text_property The property of the text.
     */
    CGText(const CGString& p_text_rk, const CGString& p_font_rk, const CGTextProperty& p_text_property);
    
    /**
     * @brief Copy constructor.
     * 
     * @param other The other CGText object.
     */
    CGText(const CGText& other);

    /**
     * @brief Move constructor.
     * 
     * @param other The other CGText object.
     */
    CGText(CGText&& other) noexcept;

    /**
     * @brief Destroy the CGText object.
     */
    virtual ~CGText() override;

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
     * @brief Get the width of the visual component.
     * 
     * @return float The width of the visual component.
     */
    virtual float GetWidth() const override;

    /**
     * @brief Get the height of the visual component
     * 
     * @return float The height of the visual component
     */
    virtual float GetHeight() const override;
};