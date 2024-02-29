#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QObject>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include "world.h"

enum class GameState { NotStarted, Autoplay, Running, GameOver };
enum class GameResult { Won, Lost, Incomplete };
enum class Item {Protagonist, Healthpack, Enemy, Door, pEnemy};

// Assuming this is your Node class
class Node : public Tile {
public:
    float f, g, h;
    bool visited;
    Node* prev;
    // std::unique_ptr<Node> prev;

    // Copy constructor from Tile
    Node(const Tile& tile) : Tile(tile), f(0.0f), g(0.0f), h(0.0f), visited(false), prev(nullptr) {
        // Initialize Node-specific fields if necessary
    }

    // Additional Node-specific methods can be added here
};

class GameModel : public QObject
{
    Q_OBJECT
public:
    GameModel();



    //getters
    const std::vector<std::unique_ptr<Enemy>>& getEnemies() const;
    const std::vector<std::unique_ptr<Tile>>& getTiles() const;
    const std::vector<std::unique_ptr<Tile>>& getHealthPacks() const;
    int getTileWidth() const;
    int getTileHeight() const;
    int getNumRows() const;
    int getNumCols() const;
    GameState getGameState() const;
    GameResult getGameResult() const;
    const std::unique_ptr<Protagonist>& getProtag() const;
    std::vector<int> getMoves() const;
    const std::vector<std::unique_ptr<Tile>>& getPoisonedTiles() const;


    void setHeuristicWeight(float newHeuristicWeight);

public slots:
    void startGame(int nr_enemies, int nr_healthpacks, int nr_xenmeies); //Initialize the world
    void moveProtagonist(int directionX, int directionY); // Logic to move protagonist
    void performXEnemyBehavior();
    void generateRandomXEnemies(int numXEnemies);
    void usePathfinder(int destX, int destY);
    void doAutoplayMove();
    Enemy* findNearestEnemy(int currentX, int currentY);
    Tile* findNearestHealthPack(int currentX, int currentY);
    void handlePoisonUpdate(int value);
    void handlePEnemyDead(int x, int y);



private slots:
    void endGame();
    void removePoisonedTilesAround(int x, int y);

    //These signals are connected to the user input first(pushbuttons, keyboard) and then they trigger would trigger the correct slots
    //This would ensure that the things first change in our model, and then updated in the view. So the slots attached to these signals
    //will have signals to pass to the controller, which will then update the view

signals:

    //Internal signals
    void generateXEnemies(int nrXenemies); //generate the specified number of X enemies
    void i_gameOver();
    void startAutoplay(); //Disable any user keyboard input from this point onwards

    //External signals (going out)
    void gameStart();//Related to the startGame slot -> Tell the view to display the world and everything in it
    void gameOver(); //To signal to the controller that game is over -> update the view
    void protagonistMoved(Item itemType);
    void updateEnemyViewAscii(const QString &asciiArt);
    void updateProtagonistViewAscii(const QString &asciiArt);
    void updateHealthPackViewAscii(const QString &asciiArt);

    void energyChanged(int newEnergy);
    void healthChanged(int newHealth);

    void updatePoisonedTiles();
    // void poisonLevelUpdated(int value);
    // void pEnemyDead();

    //External signals (coming in)
    void moveProtag(int xDirection, int yDirection); //Can also be used internally




private:
    GameState gameState;
    GameResult gameResult;
    //Game entities
    World world;

    //Static variables
    //Game dimensions
    int tileWidth;
    int tileHeight;
    int numRows;
    int numCols;

    std::unique_ptr<Protagonist> protag;
    std::vector<std::unique_ptr<Enemy>> enemies ;
    std::vector<std::unique_ptr<Tile>> tiles;
    std::vector<std::unique_ptr<Tile>> healthPacks;

    std::vector<std::unique_ptr<Tile>> poisonedTiles;
    void poisonTiles(int x, int y);

    //Functions for pathfinder and autoplay
    std::vector<Node> copyTilesToNodes();


    //Helper methods
    bool checkAllEnemiesDefeated();

    std::vector<int> moves;
    float heuristicWeight;

    //Timers
    QTimer* moveTimer;  // Timer to control autoplay actions.
    QTimer* xEnemyTimer;    // Timer to control Xenemy functions

};

#endif // GAMEMODEL_H
