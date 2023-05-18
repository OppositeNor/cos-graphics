#pragma once
#include "cos_graphics/graphics.h"
#include <vector>

class CGComponent;

/**
 * @brief The game class.
 */
class CGGame
{
    /**
     * @brief The instance of the game.
     */
    inline static CGGame* game_instance = nullptr;

    /**
     * @brief The window for the game to run on.
     */
    CGWindow* game_window = nullptr;
    
    /**
     * @brief The properties of the window.
     */
    struct WindowProperties
    {
        unsigned int width;
        unsigned int height;
        const char* title;
        CG_BOOL resizable;
        CG_BOOL fullscreen;

        WindowProperties(unsigned int p_width = 640, unsigned int p_height = 480, const char* p_title = "", 
                CG_BOOL p_fullscreen = CG_FALSE, CG_BOOL p_resizable = CG_TRUE) 
            : width(p_width), height(p_height), title(p_title), resizable(p_resizable), fullscreen(p_fullscreen)
        {}
    } window_properties;

    /**
     * @brief List of components that's in the game.
     */
    std::vector<CGComponent*> component_list;

    bool game_initialized = false;

    /**
     * @brief Constructor
     */
    CGGame();
public:

    /**
     * @brief Destroy the CGGame object
     */
    virtual ~CGGame();

    /**
     * @brief Get the instance of the game. If the game is not started, this will return nullptr.
     * 
     * @return CGGame* The instance of the game.
     */
    static CGGame* GetInstance();

    static void InitGame(
        unsigned int p_width = 640, 
        unsigned int p_height = 480, 
        const char* p_title = "", 
        CG_BOOL p_fullscreen = false, 
        CG_BOOL p_resizable = true);

    /**
     * @brief Start the game.
     */
    static void StartGame();
    
    CGWindow* GetGameWindow();

    /**
     * @brief Add component to the component list, and return the component's identifier.
     * 
     * @param p_component The component to be added.
     * @return unsigned int The component's identifier.
     */
    void AddComponent(class CGComponent* p_component);
    
    /**
     * @brief Remove component from the component list.
     * 
     * @param p_component The component to be removed.
     */
    void RemoveComponent(CGComponent* p_component);

    /**
     * @brief Set the clear color of the window
     * 
     * @param p_color The color to be set to
     */
    void SetWindowClearColor(const CGColor& p_color);
    
private:
    /**
     * @brief Called once when the game is created.
     */
    void Ready();

    /**
     * @brief Game loop.
     */
    void GameLoop();

    /**
     * @brief Update, called every frame.
     */
    void Update(float p_delta);
};