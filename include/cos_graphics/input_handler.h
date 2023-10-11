#pragma once
#include <map>
#include <vector>
/**
 * @brief The input handler class.
 * 
 * @tparam KeyType The type of the event key type
 */
template <typename GameType, typename EventKeyType>
class CGInputHandler
{
    /**
     * @brief The key map, which maps the key code to the key event.
     */
    std::map<int, std::vector<EventKeyType>> key_map;

public:
    /**
     * @brief The state of keys.
     */
    enum class KeyState
    {
        /**
         * @brief Is the key released.
         */
        Released = 0b0010,
        /**
         * @brief Is the key just released.
         * This is equivalent to (KeyState::Just & KeyState::Released)
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
         * if (GetKeyState(CGKey::A) & KeyState::Just)
         * {// the key just changed its state
         * ...}
         * if (GetKeyState(CGKey::A) == (KeyState::Just & KeyState::Pressed)   
         * {// the key just pressed. This is equivalent to KeyState::JustPressed
         * ...}
         * @endcode
         */
        Just = 0b0001
    };
private:
    /**
     * @brief The key state map.
     */
    std::map<EventKeyType, KeyState> key_state_map;

    GameType* game;
public:
    


    inline KeyState GetKeyState(EventKeyType p_key) const
    {
        return key_state_map.at(p_key);
    }

    inline void AddKey(int p_key, KeyState p_state)
    {
        key_state_map[p_key] = p_state;
    }

    inline bool IsKeyPressed(EventKeyType p_key) const
    {
        return key_state_map.at(p_key) & KeyState::Pressed;
    }

    inline bool IsKeyReleased(EventKeyType p_key) const
    {
        return key_state_map.at(p_key) & KeyState::Released;
    }

    inline bool IsKeyJustPressed(EventKeyType p_key) const
    {
        return key_state_map.at(p_key) == KeyState::JustPressed;
    }

    inline bool IsKeyJustReleased(EventKeyType p_key) const
    {
        return key_state_map.at(p_key) == KeyState::JustReleased;
    }
    
    //todo
};