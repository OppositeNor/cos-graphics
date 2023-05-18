#include "cos_graphics/game.h"
#include "cos_graphics/log.h"
#include "cos_graphics/component/component.h"

CGGame::CGGame() : window_properties(WindowProperties(640, 480, ""))
{}

CGGame::~CGGame()
{
    if (game_window != nullptr)
        CGDestroyWindow(game_window);
    for (auto i = component_list.begin(); i < component_list.end() + 1; ++i)
        delete *i;
}

CGGame* CGGame::GetInstance()
{
    CG_ERROR_COND_EXIT(game_instance == nullptr, -1, "The game instance is NULL. Please initialize the game before getting game instance.");
    return game_instance;
}

void CGGame::StartGame(unsigned int p_width, unsigned int p_height, const char* p_title, CG_BOOL p_fullscreen, CG_BOOL p_resizable)
{
    CG_PRINT("Start game called, starting game...");
    if (CGGame::game_instance != nullptr)
    {
        delete game_instance;
        return;
    }
    CGGame::game_instance = new CGGame();
    CG_PRINT("Creating window...");
    CGGame::game_instance->game_window = CGCreateWindow(
        CGGame::game_instance->window_properties.width, 
        CGGame::game_instance->window_properties.height, 
        CGGame::game_instance->window_properties.title, 
        CGGame::game_instance->window_properties.fullscreen, 
        CGGame::game_instance->window_properties.resizable);
    CG_ERROR_COND_EXIT(CGGame::game_instance->game_window == nullptr, -1, "Failed to create window");
    CG_PRINT("Window created.");
    game_instance->Ready();
    game_instance->GameLoop();
    CG_PRINT("Game exited.");
}

unsigned int CGGame::AddComponent(CGComponent* p_component)
{
    auto iter = component_list.begin();
    for (; iter <= component_list.end(); ++iter)
    {
        if (*iter == nullptr)
        {
            *iter = p_component;
            return iter - component_list.begin();
        }
    }
    component_list.insert(component_list.end(), p_component);
    return component_list.size();
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
}