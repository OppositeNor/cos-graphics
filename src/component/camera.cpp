#include "cos_graphics/component/camera.h"
#include "cos_graphics/log.h"
#include "cos_graphics/game.h"

CGCamera::CGCamera() : CGComponent()
{
    
}

void CGCamera::Use()
{
    CGGame::GetInstance()->SetMainCamera(this);
}