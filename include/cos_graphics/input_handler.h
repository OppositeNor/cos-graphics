#pragma once
#include <map>
#include <vector>
#include "cos_graphics/graphics.h"
/**
 * @brief The input handler class.
 * 
 * @tparam KeyType The type of the event key type
 */
class CGInputHandler
{
public:
    enum InputState
    {
        /**
         * @brief Is the key released.
         */
        Released = 0b0010,
        /**
         * @brief Is the key just released.
         * This is equivalent to (InputState::Just & InputState::Released)
         */
        JustReleased = 0b0011,
        /**
         * @brief Is the key pressed.
         */
        Pressed = 0b0100,
        /**
         * @brief Is the key just pressed.
         */
        JustPressed = 0b0101,

        /**
         * @brief Is the state happened just now. You can combine this with other states
         * by using bitwise or.
         * @example
         * @code
         * if (GetInputState(CGKey::A) & InputState::Just)
         * {// the key just changed its state
         * ...}
         * if (GetInputState(CGKey::A) == (InputState::Just & InputState::Pressed)   
         * {// the key just pressed. This is equivalent to InputState::JustPressed
         * ...}
         * @endcode
         */
        Just = 0b0001
    };
private:
    /**
     * @brief The key map, which maps the key code to the key event.
     */
    std::map<int, std::vector<CGString>> input_map;

    /**
     * @brief The key state map.
     */
    std::map<CGString, InputState> input_state_map;

    /**
     * @brief Set the key state to pressed.
     * If the key code is not in the input map, this function will do nothing.
     * @param p_key The key code.
     */
    void SetInputStatePressed(int p_key);

    /**
     * @brief Set the key state to released.
     * If the key code is not in the input map, this function will do nothing.
     * @param p_key The key code.
     */
    void SetInputStateReleased(int p_key);
public:
    
    /**
     * @brief Get the input state of a key.
     * 
     * @param p_key The key to get the state.
     * @return InputState The state of the key.
     */
    InputState GetInputState(const CGString& p_key) const;

    /**
     * @brief Add a input to the input handler.
     * 
     * @param input_name The name of the input.
     * @param p_key The key code of the input.
     */
    void AddInput(const CGString& input_name, int p_key);

    /**
     * @brief Is the input pressed.
     * 
     * @param p_key The name of the input.
     * @return true The input is pressed.
     * @return false The input is not pressed.
     */
    bool IsInputPressed(const CGString& p_key) const;

    /**
     * @brief Is the input released.
     * 
     * @param p_key The name of the input.
     * @return true The input is released.
     * @return false The input is not released.
     */
    bool IsInputReleased(const CGString& p_key) const;

    /**
     * @brief Is the input just pressed.
     * 
     * @param p_key The name of the input.
     * @return true The input is just pressed.
     * @return false The input is not just pressed.
     */
    bool IsInputJustPressed(const CGString& p_key) const;

    /**
     * @brief Is the input just released.
     * 
     * @param p_key The name of the input.
     * @return true The input is just released.
     * @return false The input is not just released.
     */
    bool IsInputJustReleased(const CGString& p_key) const;
    
    /**
     * @brief The key function called by the game.
     * 
     * @param p_key The key code that recieves the action.
     * @param p_action The action of the key. This should be one of CG_PRESS, CG_RELEASE, CG_REPEAT.
     */
    void KeyFunc(int p_key, int p_action);
};