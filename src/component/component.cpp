#include "cos_graphics/component/component.h"
#include "cos_graphics/game.h"

CGComponent::CGTransform::CGTransform(const CGVector2& p_position, float p_rotation, const CGVector2& p_scale)
    : position(p_position), rotation(p_rotation), scale(p_scale)
{
    
}

CGComponent::CGComponent()
{
    CGGame::GetInstance()->AddComponent(this);
    Ready();
}

CGComponent::~CGComponent()
{
    CGGame::GetInstance()->RemoveComponent(this);
}

void CGComponent::Tick(double p_delta_time)
{
    Update(p_delta_time);
}