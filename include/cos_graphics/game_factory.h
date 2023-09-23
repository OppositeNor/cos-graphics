#pragma once
/**
 * @brief Contructor of the game object.
 */
class CGGame;
class CGGameFactory
{
    friend CGGame;
public:
    CGGameFactory();
    virtual ~CGGameFactory();

private:
    /**
     * @brief Create a instance of the game.
     * 
     * @return CGGame* 
     */
    virtual CGGame* CreateGame() const;
};