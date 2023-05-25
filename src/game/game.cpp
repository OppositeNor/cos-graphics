#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/component/component.h"

CGGame::CGGame()
{
    
}

CGGame::~CGGame()
{
    if (game_window != nullptr)
        CGDestroyWindow(game_window);
    for (auto i = component_list.begin(); i < component_list.end() + 1; ++i)
        delete (*i);
    game_initialized = false;
}

CGGame* CGGame::GetInstance()
{
    CG_ERROR_COND_EXIT(game_instance == nullptr, -1, "The game instance is NULL. Please initialize the game before getting game instance.");
    return game_instance;
}

void CGGame::InitGame(unsigned int p_width, unsigned int p_height, const char* p_title, CG_BOOL p_fullscreen, CG_BOOL p_resizable)
{
    CG_PRINT("Start game called, starting game...");
    if (CGGame::game_instance != nullptr)
    {
        delete game_instance;
        return;
    }
    CGGame::game_instance = new CGGame();
    CGGame::game_instance->window_properties = WindowProperties(p_width, p_height, p_title, p_fullscreen, p_resizable);
    CG_PRINT("Creating window...");
    CGGame::game_instance->game_window = CGCreateWindow(
        CGGame::game_instance->window_properties.width, 
        CGGame::game_instance->window_properties.height, 
        CGGame::game_instance->window_properties.title, 
        CGGame::game_instance->window_properties.fullscreen, 
        CGGame::game_instance->window_properties.resizable);
    CG_ERROR_COND_EXIT(CGGame::game_instance->game_window == nullptr, -1, "Failed to create window");
    CG_PRINT("Window created.");
    CG_PRINT("Game initialized.");
    game_instance->game_initialized = true;
}

void CGGame::StartGame()
{
    CG_ERROR_CONDITION(game_instance == nullptr || !game_instance->game_initialized, "Game is not initialized. Please initialize the game before starting the game.");
    game_instance->Ready();
    game_instance->GameLoop();
    CG_PRINT("Game exited.");
}

void CGGame::AddComponent(CGComponent* p_component)
{
    component_list.insert(component_list.end(), p_component);
}

void CGGame::RemoveComponent(CGComponent* p_component)
{
    auto iter = component_list.begin();
    for (; iter < component_list.end(); ++iter)
    {
        if (*iter == p_component)
        {
            component_list.erase(iter);
            return;
        }
    }
}

void CGGame::SetWindowClearColor(const CGColor& p_color)
{
    CG_ERROR_CONDITION(game_instance == nullptr || !game_instance->game_initialized, "Game is not initialized. Please initialize the game before setting the window clear color.");
    CGSetClearScreenColor(p_color);
}

CGWindow* CGGame::GetGameWindow()
{
    return game_window;
}

void CGGame::Ready()
{


}

void CGGame::Update(float p_delta)
{
    for (auto i : component_list)
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
        Update(delta);
        CGWindowDraw(game_window);
        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }
    CGTerminateGraphics();
}