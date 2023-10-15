#pragma once
/**
 * @brief Contructor of the game object.
 */
class CGGame;
class CGGameFactory
{
    friend CGGame;
protected:
    /**
     * @brief Create a instance of the game.
     * 
     * @return CGGame* 
     */
    virtual CGGame* CreateGame() const;

    /**
     * @brief Create a root component of the game.
     * 
     * @return CGComponent* 
     */
    virtual class CGComponent* CreateRootComponent() const;

public:
    CGGameFactory();
    virtual ~CGGameFactory();

};