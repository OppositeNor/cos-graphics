#include "cos_graphics/game.h"
#include "cos_graphics/log.h"

CGGame::CGGame() : window_properties(WindowProperties(640, 480, ""))
{}

CGGame::~CGGame()
{
    if (game_window != nullptr)
        CGDestroyWindow(game_window);
}

CGGame* CGGame::GetInstance()
{
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

void CGGame::Ready()
{
    /**
     * TODO: game ready
     * 
     */
}

void CGGame::Update(float delta)
{
    /**
     * TODO: game update
     */
}

void CGGame::Render()
{
    CGTickRenderStart(game_window);
    /**
     * TODO: game render
     */
    
    CGTickRenderEnd();
}

void CGGame::GameLoop()
{
    double tick_end_time = CGGetCurrentTime();
    while (!CGShouldWindowClose(game_window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();
        Update(delta);
        Render();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }
}