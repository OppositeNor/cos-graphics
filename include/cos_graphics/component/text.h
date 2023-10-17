#pragma once
#include "cos_graphics/component/texture_container.h"

/**
 * @brief A text that can be rendered on the screen.
 */
class CGText : public CGTextureContainer
{
    CG_COMPONENT(CGText)
private:
    CGString text;
    CGString font_rk;
    CGTextProperty text_property;

    void UpdateTextImage();
protected:
    virtual void Draw(float p_delta) final override;

public:
    /**
     * @brief Constructor of CGText.
     * 
     * @param p_text The text that the component will draw. If p_is_text_rk is 
     * set to true, then p_text is the resource key of the text.
     * @param p_text_property The property of the text.
     * @param p_is_text_rk Is p_text a resource key of the text.
     */
    CGText(const CGString& p_text, const CGTextProperty& p_text_property, bool p_is_text_rk = false);

    /**
     * @brief Constructor of CGText.
     * 
     * @param p_text The text that the component will draw. If p_is_text_rk is 
     * set to true, then p_text is the resource key of the text.
     * @param p_font_rk The resource key of the font.
     * @param p_text_property The property of the text.
     * @param p_is_text_rk Is p_text a resource key of the text.
     */
    CGText(const CGString& p_text, const CGString& p_font_rk, const CGTextProperty& p_text_property, bool p_is_text_rk = false);
    
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
};