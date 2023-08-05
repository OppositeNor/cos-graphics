#pragma once
#include "cos_graphics/component/visual_component.h"
#include "cos_graphics/resource.h"
#include <vector>
#include <map>
#include <string>
#include <functional>

using CGAnimationMap = std::map<std::string, std::vector<CGVisualImage*>>;
using CGAnimationPair = std::pair<std::string, std::vector<CGVisualImage*>>;

/**
 * @brief A component that displays an animation.
 * @warning UNTESTED and BUGGY
 */
class CGAnimationSprite : public CGVisualComponent
{
    CG_COMPONENT_TYPE(CGAnimationSprite)
protected:

    void Draw(float p_delta) override;

    /**
     * @brief How many seconds each frame is going to be displayed.
     */
    float frame_duration;

    /**
     * @brief The current animation that is playing;
     */
    std::string current_animation;

    /**
     * @brief An internal clock of the animation.
     */
    float clock;

    /**
     * @brief The map the animations' textures.
     * @tparam std::string The name of the animation.
     * @tparam std::std::vector<CGVisualImage*> The list of textures of the animation.
     */
    CGAnimationMap animation_map;

    /**
     * @brief Is the animation playing.
     */
    bool is_playing = false;

    /**
     * @brief The function that will be called after the animation is finished.
     * @param CGAnimationSprite* The animation that is finished.
     */
    std::function<void(CGAnimationSprite*)> animation_finish_callback;

public:

    virtual ~CGAnimationSprite();

    /**
     * @brief Construct a new CGAnimationSprite::CGAnimationSprite object
     * 
     * @param p_fps The frame rate of the animation.
     * @param p_position The position of the animation is going to be displayed.
     */
    explicit CGAnimationSprite(float p_fps = 5.0f);

    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_animation_map The map the animations' textures.
     * @param p_fps The frame rate of the animation.
     * @param p_start_frame The frame that the animation is going to start.
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(CGAnimationMap& p_animation_map, std::string p_default_animation, float p_fps = 5.0f);
    
    /**
     * @brief Construct a new CGAnimationSprite object
     * 
     * @param p_animation_map The map the animations' textures.
     * @param p_fps The frame rate of the animation.
     * @param p_start_frame The frame that the animation is going to start.
     * @param p_position The position of the animation is going to be displayed.
     */
    CGAnimationSprite(CGAnimationMap&& p_animation_map, std::string p_default_animation, float p_fps = 5.0f);

    

    /**
     * @brief Add an animation to the animation map.
     * 
     * @param p_animation_name The name of the animation.
     * @param p_animation The list of textures of the animation.
     */
    void AddAnimation(const std::string& p_animation_name, const std::vector<CGVisualImage*>& p_animation);

    /**
     * @brief Add an animation to the animation map.
     * 
     * @param p_animation_pair The pair of the animation name and the list of textures of the animation.
     */
    void AddAnimation(const CGAnimationPair& p_animation_pair);
    
    /**
     * @brief Play the current animation.
     */
    void Play();
    /**
     * @brief Play the current animation from start.
     */
    void PlayFromStart();
    /**
     * @brief Play the animation from start.
     * 
     * @param p_animation_name The animation to be played.
     */
    void PlayFromStart(std::string p_animation_name);

    /**
     * @brief Set the animation finish callback function.
     * 
     * @param animation_finish_callback The function that will be called after the animation is finished.
     */
    void SetAnimationFinishCallback(const std::function<void(CGAnimationSprite*)>& p_animation_finish_callback);

    /**
     * @brief Set the animation finish callback function.
     * 
     * @param animation_finish_callback The function that will be called after the animation is finished.
     */
    void SetAnimationFinishCallback(const std::function<void(CGAnimationSprite*)>&& p_animation_finish_callback);
};