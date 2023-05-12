#pragma once
/**
 * @brief Component is a base class for all the items that exists in the game world.
 * 
 */
class CGComponent
{
public:
    CGComponent();
    
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

};