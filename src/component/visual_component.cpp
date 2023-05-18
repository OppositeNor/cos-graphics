#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/game.h"

CGVisualComponent::CGVisualComponent(const CGVector2& p_position) : CGComponent(), position(p_position)
{
    window = CGGame::GetInstance()->GetGameWindow();
}

void CGVisualComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
    Draw();
}