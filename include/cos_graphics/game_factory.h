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

    /**
     * @brief Create a root component of the game.
     * 
     * @return CGComponent* 
     */
    virtual class CGComponent* CreateRootComponent() const;
};