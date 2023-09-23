#pragma once
#include <cos_graphics/component/sprite.h>
#include <cos_graphics/component/text.h>
#include <memory>

class MainMenu : public CGSprite
{
    CG_COMPONENT(MainMenu)

    std::unique_ptr<CGText> title;
public:

    virtual void Ready() override
    {
        title = std::make_unique<CGText>(CGSTR("test_text"), CGConstructTextProperty(60, 60, 30, 10));
        title->GetTransform().position.y = 100;
    }
    virtual void Update(double p_delta_time) override
    {
    }

};