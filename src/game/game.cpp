#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/component/component.h"
#include "cos_graphics/component/camera.h"

CGGame::CGGame() : window_properties(WindowProperties(640, 480, CGSTR(""), CG_FALSE, CG_FALSE, CG_FALSE, CG_FALSE, CG_FALSE))
{
    
}

CGGame::~CGGame()
{
    game_initialized = false;
}

CGGame* CGGame::GetInstance()
{
    CG_ERROR_COND_EXIT(game_instance == nullptr, -1, CGSTR("The game instance is NULL. Please initialize the game before getting game instance."));
    return game_instance;
}

void CGGame::InitGame(unsigned int p_width, unsigned int p_height, CGString p_title, 
    CG_BOOL p_fullscreen, CG_BOOL p_resizable, CG_BOOL p_boarderless, CG_BOOL p_transparent, CG_BOOL p_topmost)
{
    CG_PRINT(CGSTR("Initializing game..."));
    if (CGGame::game_instance != nullptr)
    {
        delete game_instance;
        game_instance = nullptr;
    }
    CGGame::game_instance = new CGGame();
    CGGame::game_instance->window_properties = WindowProperties(p_width, p_height, p_title, p_fullscreen, 
        p_resizable, p_boarderless, p_transparent, p_topmost);
    CG_PRINT(CGSTR("Creating window..."));
    
    CGWindowSubProperty window_sub_property;
    window_sub_property.use_full_screen = p_fullscreen;
    window_sub_property.resizable = p_resizable;
    window_sub_property.boarderless = p_boarderless;
    window_sub_property.transparent = p_transparent;
    window_sub_property.topmost = p_topmost;

    CGGame::game_instance->game_window = CGCreateWindow(
        p_width, 
        p_height, 
        p_title.c_str(),
        window_sub_property);
    
    CG_ERROR_COND_EXIT(CGGame::game_instance->game_window == nullptr, -1, CGSTR("Failed to create window"));
    CG_PRINT(CGSTR("Window created."));
    CG_PRINT(CGSTR("Game initialized."));
    game_instance->game_initialized = true;
    game_instance->main_camera = new CGCamera();
}

void CGGame::InitGame(unsigned int p_width, unsigned int p_height, CGString p_title, 
    CG_BOOL p_fullscreen, CG_BOOL p_resizable)
{
    InitGame(p_width, p_height, p_title, p_fullscreen, p_resizable, CG_FALSE, CG_FALSE, CG_FALSE);
}

void CGGame::InitGame(unsigned int p_width, unsigned int p_height, CGString p_title)
{
    InitGame(p_width, p_height, p_title, CG_FALSE, CG_TRUE, CG_FALSE, CG_FALSE, CG_FALSE);
}

void CGGame::StartGame()
{
    CG_ERROR_CONDITION(game_instance == nullptr || !game_instance->game_initialized, CGSTR("Game is not initialized. Please initialize the game before starting the game."));
    game_instance->Ready();
    game_instance->GameLoop();
}

void CGGame::ExitGame()
{
    CG_ERROR_CONDITION(game_instance == nullptr || !game_instance->game_initialized, CGSTR("Game is not initialized. Please initialize the game before exiting the game."));
    delete game_instance;
    game_instance = nullptr;
    CGTerminateGraphics();
    CG_PRINT(CGSTR("Game exited."));
}

void CGGame::AddComponent(CGComponent* p_component)
{
    component_list.insert(component_list.end(), p_component);
    component_prepare_list.insert(component_prepare_list.end(), p_component);
}

void CGGame::RemoveComponent(CGComponent* p_component)
{
    auto iter = component_list.begin();
    for (auto iter = component_list.begin(); iter < component_list.end(); ++iter)
    {
        if (*iter != p_component)
            continue;
        else
        {
            component_list.erase(iter);
            break;
        }
    }
    for (auto iter = component_prepare_list.begin(); iter < component_prepare_list.end(); ++iter)
    {
        if (*iter == p_component)
        {
            component_prepare_list.erase(iter);
            return;
        }
    }
}

void CGGame::SetWindowClearColor(const CGColor& p_color)
{
    CG_ERROR_CONDITION(game_instance == nullptr || !game_instance->game_initialized, CGSTR("Game is not initialized. Please initialize the game before setting the window clear color."));
    CGSetClearScreenColor(p_color);
}

const CGCamera* CGGame::GetMainCamera() const noexcept
{
    return main_camera;
}

CGCamera* CGGame::GetMainCamera() noexcept
{
    return main_camera;
}

void CGGame::SetMainCamera(CGCamera* p_camera) noexcept
{
    main_camera = p_camera;
}

const CGWindow* CGGame::GetGameWindow() const noexcept
{
    return game_window;
}

CGWindow* CGGame::GetGameWindow() noexcept
{
    return game_window;
}

void CGGame::Tick(float p_delta)
{
    while (!component_prepare_list.empty())
    {
        (*component_prepare_list.begin())->Ready();
        component_prepare_list.erase(component_prepare_list.begin());
    }
    component_prepare_list.clear();
    Update(p_delta);
    for (auto& i : component_list)
        i->Tick(p_delta);
}

void CGGame::GameLoop()
{
    double tick_end_time = CGGetCurrentTime();
    while (!CGShouldWindowClose(game_window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();
        CGTickRenderStart(game_window);
        Tick(delta);
        CGWindowDraw(game_window);
        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }
}