#pragma once
#include "cos_graphics/graphics.h"
#include <vector>
#include <functional>

class CGComponent;
class CGCamera;

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
        std::string title;
        CG_BOOL resizable;
        CG_BOOL fullscreen;
        CG_BOOL boarderless;
        CG_BOOL transparent;
        CG_BOOL topmost;

        WindowProperties(unsigned int p_width, unsigned int p_height, std::string p_title, 
                CG_BOOL p_fullscreen = CG_FALSE, CG_BOOL p_resizable = CG_TRUE) noexcept
            : WindowProperties(p_width, p_height, p_title, p_fullscreen, p_resizable, CG_FALSE, CG_FALSE, CG_FALSE)
        {}

        WindowProperties(unsigned int p_width, unsigned int p_height, std::string p_title, 
                CG_BOOL p_fullscreen, CG_BOOL p_resizable, CG_BOOL p_boarderless, CG_BOOL p_transparent, CG_BOOL p_topmost) noexcept
            : width(p_width), height(p_height), title(p_title), resizable(p_resizable), fullscreen(p_fullscreen),
            boarderless(p_boarderless), transparent(p_transparent), topmost(p_topmost)
        {}
    } window_properties;

    /**
     * @brief List of components that's in the game.
     */
    std::vector<CGComponent*> component_list;

    bool game_initialized = false;

    /**
     * @brief The main camera for the game.
     */
    CGCamera* main_camera = nullptr;

    /**
     * @brief Constructor
     */
    CGGame();

    /**
     * @brief Marks that is the game terminating and preparing to release resources.
     */
    bool game_terminating = false;
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
        unsigned int p_width, 
        unsigned int p_height, 
        std::string p_title,
        CG_BOOL p_fullscreen,
        CG_BOOL p_resizable,
        CG_BOOL boarderless, 
        CG_BOOL transparent, 
        CG_BOOL topmost);
    
    static void InitGame(
        unsigned int p_width, 
        unsigned int p_height, 
        std::string p_title, 
        CG_BOOL p_fullscreen, 
        CG_BOOL p_resizable);

    static void InitGame(
        unsigned int p_width, 
        unsigned int p_height, 
        std::string p_title);

    /**
     * @brief Start the game.
     */
    static void StartGame();

    /**
     * @brief Exit the game.
     */
    static void ExitGame();

    /**
     * @brief Get the instance of the game window.
     * 
     * @return CGWindow* The instance of the game window.
     */
    const CGWindow* GetGameWindow() const noexcept;
    
    /**
     * @brief Get the instance of the game window.
     * 
     * @return CGWindow* The instance of the game window.
     */
    CGWindow* GetGameWindow() noexcept;

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

    /**
     * @brief Get the Main Camera object
     * 
     * @return CGCamera* The main camera object.
     */
    const CGCamera* GetMainCamera() const noexcept;
    
    /**
     * @brief Get the Main Camera object
     * 
     * @return CGCamera* The main camera object.
     */
    CGCamera* GetMainCamera() noexcept;

    /**
     * @brief Use the camera as the main camera.
     * 
     * @param p_camera The camera that the main camera will be set to.
     */
    void SetMainCamera(CGCamera* p_camera) noexcept;

    /**
     * @brief Check if the game is terminating and releasing component resource.
     * 
     * @return true The game is terminating and self-releasing component resources is not allowed.
     * @return false The game is not terminating and self-releasing resources is allowed.
     */
    bool IsGameTerminating() const noexcept;
    
private:

    /**
     * @brief Game loop.
     */
    void GameLoop();

    /**
     * @brief Update, called every frame.
     */
    void Tick(float p_delta);

protected:
    /**
     * @brief Called once when the game is created.
     */
    virtual void Ready() {}

    /**
     * @brief Called every frame.
     * 
     * @param p_delta The difference in time between frames.
     */
    virtual void Update(float p_delta) {}
};