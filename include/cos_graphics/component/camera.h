#pragma once
#include "cos_graphics/component/component.h"

/**
 * @brief The camera of the game.
 * @details The camera can be used to put the view of the game 
 * on a specific position.
 */
class CGCamera : public CGComponent
{
    CG_COMPONENT(CGCamera)
public:
    CGCamera();

    /**
     * @brief Use this camera as the main camera of the game.
     */
    void Use();
};