#include "cos_graphics/input_handler.h"

void CGInputHandler::SetInputStatePressed(int p_key)
{
    auto key = input_map.find(p_key);
    if (key == input_map.end())
        return;
    auto key_list = key->second;
    for (auto iter = key_list.begin(); iter != key_list.end(); ++iter)
    {
        input_state_map[*iter] = input_state_map[*iter] & InputState::Released ? InputState::JustPressed : InputState::Pressed;
    }
}

void CGInputHandler::SetInputStateReleased(int p_key)
{
    auto key = input_map.find(p_key);
    if (key == input_map.end())
        return;
    auto key_list = key->second;
    for (auto iter = key_list.begin(); iter != key_list.end(); ++iter)
    {
        input_state_map[*iter] = input_state_map[*iter] & InputState::Pressed ? InputState::JustReleased : InputState::Released;
    }
}

CGInputHandler::InputState CGInputHandler::GetInputState(const CGString& p_key) const
{
    return input_state_map.at(p_key);
}

void CGInputHandler::AddInput(const CGString& input_name, int p_key)
{
    input_map[p_key].push_back(input_name);
    input_state_map[input_name] = InputState::Released;
}

void CGInputHandler::ClearInput()
{
    input_map.clear();
    input_state_map.clear();
}

bool CGInputHandler::IsInputPressed(const CGString& p_key) const
{
    return input_state_map.at(p_key) & InputState::Pressed;
}

bool CGInputHandler::IsInputReleased(const CGString& p_key) const
{
    return input_state_map.at(p_key) & InputState::Released;
}

bool CGInputHandler::IsInputJustPressed(const CGString& p_key) const
{
    return input_state_map.at(p_key) == InputState::JustPressed;
}

bool CGInputHandler::IsInputJustReleased(const CGString& p_key) const
{
    return input_state_map.at(p_key) == InputState::JustReleased;
}

void CGInputHandler::KeyFunc(int p_key, int p_action)
{
    switch (p_action)
    {
    case CG_RELEASE:
        SetInputStateReleased(p_key);
        break;
    case CG_PRESS:
        SetInputStatePressed(p_key);
        break;
    default:
        break;
    }
}