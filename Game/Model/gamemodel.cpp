#include "gamemodel.h"
#include "Model/behaviorstrategy.h"
#include "Model/growingbehavior.h"
#include "Model/movingbehavior.h"
#include "Model/teleportbehavior.h"
#include "Model/xenemy.h"
#include "pathfinder_class.h"

GameModel::GameModel() {
    gameState = GameState::NotStarted;
}

const std::vector<std::unique_ptr<Enemy>>& GameModel::getEnemies() const
{
    return enemies;
}

const std::vector<std::unique_ptr<Tile>>& GameModel::getTiles() const
{
    return tiles;
}

const std::vector<std::unique_ptr<Tile>>& GameModel::getHealthPacks() const
{
    return healthPacks;
}

int GameModel::getTileWidth() const
{
    return tileWidth;
}

int GameModel::getTileHeight() const
{
    return tileHeight;
}

int GameModel::getNumRows() const
{
    return numRows;
}

int GameModel::getNumCols() const
{
    return numCols;
}

void GameModel::moveProtagonist(int xDirection, int yDirection) {
    if(gameState == GameState::Running || gameState == GameState::Autoplay){
        if(protag->getHealth() <= 0 || protag->getEnergy() <= 0){
            emit i_gameOver();
            return;
        }
        int newX, newY ;
        if(xDirection == 1 || xDirection == -1 || yDirection == 1 ||yDirection == -1 ){
            newX = protag->getXPos() + xDirection;
            newY = protag->getYPos() + yDirection;
        }else{
            newX = xDirection;
            newY = yDirection;
        }
        if (newX < 0 || newX >= numCols) {
            newX = qBound(0, newX, numCols - 1);
        }if (newY < 0 || newY >= numRows) {
            newY = qBound(0, newY, numRows -1);
        }

        // Check if the new tile has infinite energy
        if(std::isinf(tiles[numRows*newY + newX]->getValue())){
            // Do not allow the move, as it's like a brick wall
            return;
        }

        protag->setXPos(newX);
        protag->setYPos(newY);

        //Check for energy
        auto newEnergy = protag->getEnergy() - tiles[numRows*newY + newX]->getValue();
        if(newEnergy <= 0 ){
            protag->setEnergy(0);
            emit energyChanged(0);
            emit i_gameOver();
            return;
        }
        else{
            protag->setEnergy(newEnergy);
            emit energyChanged(newEnergy);
        }

        //Check for enemy on the tile
        for (auto it = enemies.begin(); it != enemies.end(); ++it) {
            auto &enemy = *it;
            if(enemy->getXPos() == protag->getXPos() && enemy->getYPos() == protag->getYPos() && !enemy->getDefeated()){
                if(PEnemy* pEnemy = dynamic_cast<PEnemy*>(enemy.get())){
                    if(protag->getHealth() - static_cast<int>(enemy->getValue()*0.75) < 0){
                        protag->setHealth(0);
                        emit healthChanged(0);
                        emit i_gameOver();
                        return; // Exit the function early as the game is over
                    }
                    pEnemy->poison();
                    int xPos = pEnemy->getXPos();
                    int yPos = pEnemy->getYPos();
                    connect(pEnemy, &PEnemy::poisonLevelUpdated, this, &GameModel::handlePoisonUpdate);
                    connect(pEnemy, &PEnemy::dead, this, [this, xPos, yPos]() {
                        this->handlePEnemyDead(xPos, yPos);
                    });
                    protag->setHealth(protag->getHealth() - static_cast<int>(enemy->getValue()*0.75));
                    emit healthChanged(protag->getHealth());
                    emit protagonistMoved(Item::pEnemy);
                    poisonTiles(enemy->getXPos(), enemy->getYPos());

                }
                else{
                    // If the protagonist's health will be depleted, end the game
                    if(protag->getHealth() - enemy->getValue() < 0){
                        protag->setHealth(0);
                        emit healthChanged(0);
                        qDebug() << "Gonna dieeee";
                        emit i_gameOver();
                        return; // Exit the function early as the game is over
                    } else {
                        // Otherwise, reduce the protagonist's health and mark the enemy as defeated
                        protag->setHealth(protag->getHealth() - enemy->getValue());
                        emit healthChanged(protag->getHealth());
                        qDebug() << "Updating health now, current health is:" << protag->getHealth();
                        enemy->setDefeated(true);
                        emit protagonistMoved(Item::Enemy);
                        if(checkAllEnemiesDefeated()){
                            emit i_gameOver();
                            return;
                        }
                    }
                    break; // Exit the loop as we've already found the enemy on this tile
                }
            }
        }

        //Check for health pack on the tile
        for (auto it = healthPacks.begin(); it != healthPacks.end();) {
            auto& healthPack = *it;
            if (newX == healthPack->getXPos() && newY == healthPack->getYPos()) {
                // Adjust the protagonist's health
                int newHealth = protag->getHealth() + healthPack->getValue();
                protag->setHealth(std::min(newHealth, 100));  // Ensures health doesn't exceed 100
                emit healthChanged(std::min(newHealth, 100));
                // Remove the health pack from the list
                it = healthPacks.erase(it);
                emit protagonistMoved(Item::Healthpack);
                return;
            } else {
                ++it;  // Only increment the iterator if you haven't erased an element
            }
        }
        emit protagonistMoved(Item::Protagonist); //This should trigger the view to be updated, with the protagonist moved
    }
    else if(gameState == GameState::GameOver){
        emit i_gameOver();
        return;
    }

}

void GameModel::performXEnemyBehavior()
{
    if(gameState == GameState::Running || gameState == GameState::Autoplay){
        for (auto& enemy : enemies) {
            if (XEnemy* xEnemy = dynamic_cast<XEnemy*>(enemy.get())) {
                if (TeleportBehavior* teleportBehavior = dynamic_cast<TeleportBehavior*>(xEnemy->getBehavior())) {
                    qDebug() << "teleports!!!";
                    xEnemy->performBehavior(numCols, numRows, enemies);
                    //Update?
                }else if(GrowingBehavior* growingBehavior = dynamic_cast<GrowingBehavior*>(xEnemy->getBehavior())){
                    qDebug() << "grows!!!";
                    xEnemy->performBehavior(numCols, numRows, enemies);
                    //Update?
                }
            }
        }
        if(checkAllEnemiesDefeated()){
            emit i_gameOver();
            return;
        }
        else{
            emit protagonistMoved(Item::Enemy);
        }
    }
}

void GameModel::generateRandomXEnemies(int numXEnemies)
{
    for (int i = 0; i < numXEnemies; ++i)
    {
        // Random values for position and strength
        int xPosition = rand() % numCols;
        int yPosition = rand() % numRows;
        float sStrength = static_cast<float>(rand() % 100);

        // Randomly choose a behavior
        BehaviorStrategy* strategy = nullptr;
        int randomBehavior = rand() % 2; // this value can be extended - new behaviors

        std::unique_ptr<XEnemy> newEnemy = std::make_unique<XEnemy>(xPosition, yPosition, sStrength);

        switch (randomBehavior)
        {
        case 0:
            strategy = new GrowingBehavior(newEnemy.get());
            break;
        case 1:
            strategy = new TeleportBehavior(newEnemy.get());
            break;
        // case 2:
        //     strategy = new MovingBehavior(newEnemy.get());
            break;
        default:
            break;
        }

        newEnemy->setBehavior(strategy);
        enemies.push_back(std::move(newEnemy));
    }
}



bool GameModel::checkAllEnemiesDefeated()
{
    bool allDefeated = true;  // Flag to track if all enemies are defeated

    for (const auto& enemy : enemies) {
        if (!enemy->getDefeated()) {
            // If any enemy is not defeated, set the flag to false and break out of the loop
            allDefeated = false;
            break;
        }
    }

    return allDefeated;
}

void GameModel::setHeuristicWeight(float newHeuristicWeight)
{
    heuristicWeight = newHeuristicWeight;
}

std::vector<int> GameModel::getMoves() const
{
    return moves;
}

const std::unique_ptr<Protagonist>& GameModel::getProtag() const
{
    return protag;
}

void GameModel::startGame(int nr_enemies, int nr_healthpacks, int nr_xenemies){

    connect(this, &GameModel::generateXEnemies, this, &GameModel::generateRandomXEnemies);
    world.createWorld(":/Images/worldmap.png", nr_enemies, nr_healthpacks);
    tiles = world.getTiles();
    // generateRandomXEnemies(nr_xenemies);
    enemies = world.getEnemies();
    healthPacks = world.getHealthPacks();
    protag = world.getProtagonist();
    numRows = world.getRows();
    numCols = world.getCols();
    emit generateXEnemies(nr_xenemies);
    tileWidth = 25;
    tileHeight = 25;
    qDebug() << "numRows:" <<numRows <<" numCols:"<< numCols << "tileWidth" << tileWidth << "tileHeight" << tileHeight;
    gameState = GameState::Running;
    //Signal->slot to end the game
    connect(this, &GameModel::i_gameOver, this, &GameModel::endGame);
    //Xenemy timer
    xEnemyTimer = new QTimer(this);
    connect(xEnemyTimer, &QTimer::timeout, this, &GameModel::performXEnemyBehavior);
    xEnemyTimer->setInterval(2000);
    xEnemyTimer->start();
    emit energyChanged(100);
    emit healthChanged(100);

}

void GameModel::endGame(){
    gameState = GameState::GameOver;
    xEnemyTimer->stop();
    poisonedTiles.clear();
    disconnect(this, &GameModel::generateXEnemies, this, &GameModel::generateRandomXEnemies);
    disconnect(xEnemyTimer, &QTimer::timeout, this, &GameModel::performXEnemyBehavior);
    if(checkAllEnemiesDefeated()){
        gameResult = GameResult::Won;
    }
    else{
        gameResult = GameResult::Lost;
    }
    emit gameOver();
}

void GameModel::removePoisonedTilesAround(int x, int y)
{
    std::vector<std::unique_ptr<Tile>>::iterator it = poisonedTiles.begin();
    while (it != poisonedTiles.end()) {
        // Check if tile is a poisoned tile
        if (std::abs((*it)->getXPos() - x) <= 1 && std::abs((*it)->getYPos() - y) <= 1) {
            it = poisonedTiles.erase(it); // Remove the tile
        } else {
            ++it;
        }
    }
}

const std::vector<std::unique_ptr<Tile>>& GameModel::getPoisonedTiles() const{
    return poisonedTiles;
}

void GameModel::poisonTiles(int x, int y) {
    // Clear the previous poisoned tiles
    // poisonedTiles.clear();

    // Define the relative positions of the surrounding tiles
    std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {0, -1}, {1, -1}, // Top row
        {-1, 0},  {0, 0},  {1, 0},  // Middle row (excluding the center)
        {-1, 1},  {0, 1},  {1, 1}   // Bottom row
    };

    // Iterate through each direction and add the tile if it's within bounds
    for (const auto& dir : directions) {
        int newX = x + dir.first;
        int newY = y + dir.second;

        // Check if the new position is within the bounds of the game world
        if (newX >= 0 && newX < numCols && newY >= 0 && newY < numRows) {
            // Create a copy of the tile and add it to the poisonedTiles vector
            const Tile* originalTile = tiles[newY * numCols + newX].get();
            if (originalTile) {
                std::unique_ptr<Tile> poisonedTile = std::make_unique<Tile>(*originalTile);
                poisonedTiles.push_back(std::move(poisonedTile));
            }
        }
    }
    emit updatePoisonedTiles();
    for(const auto & tile : poisonedTiles){
        qDebug() << "Tiles are:" << tile->getXPos() << ", " << tile->getYPos();
    }
}

GameResult GameModel::getGameResult() const
{
    return gameResult;
}

GameState GameModel::getGameState() const {
    return gameState;
}

double calculateDistance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

Enemy* GameModel::findNearestEnemy(int currentX, int currentY) {
    Enemy* nearestEnemy = nullptr;
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& enemy : enemies) {
        if (!enemy->getDefeated()) {
            int enemyX = enemy->getXPos();
            int enemyY = enemy->getYPos();

            if (enemyX == currentX && enemyY == currentY) {
                continue;
            }
            if (PEnemy* pEnemy = dynamic_cast<PEnemy*>(enemy.get())) {
                if (pEnemy->getPoisonLevel() < pEnemy->getValue()) {
                    // Skip this enemy as it's a PEnemy that's still poisoning
                    continue;
                }
            }

            double distance = calculateDistance(currentX, currentY, enemyX, enemyY);
            if (distance < minDistance) {
                minDistance = distance;
                nearestEnemy = enemy.get();
            }
        }
    }
    return nearestEnemy;
}

Tile* GameModel::findNearestHealthPack(int currentX, int currentY) {
    Tile* nearestHealthPack = nullptr;
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& healthPack : healthPacks) {
        int healthX = healthPack->getXPos();
        int healthY = healthPack->getYPos();
        double distance = calculateDistance(currentX, currentY, healthX, healthY);
        if (distance < minDistance) {
            minDistance = distance;
            nearestHealthPack = healthPack.get();
        }
    }
    return nearestHealthPack;
}

void GameModel::handlePoisonUpdate(int value)
{
    // emit poisonLevelUpdated(value);
    bool isProtagOnPoisonedTile = false;
    int protagX = protag->getXPos();
    int protagY = protag->getYPos();
    for (const auto& poisonedTile : poisonedTiles) {
        if (poisonedTile && poisonedTile->getXPos() == protagX && poisonedTile->getYPos() == protagY) {
            isProtagOnPoisonedTile = true; // Protagonist is on a poisoned tile
        }
    }
    if(isProtagOnPoisonedTile){
        if(protag->getHealth() - value <= 0){
            protag->setHealth(0);
            emit healthChanged(0);
            emit i_gameOver();
        }
        else {
            protag->setHealth(protag->getHealth() - value);
            emit healthChanged(protag->getHealth());
        }

    }
}

void GameModel::handlePEnemyDead(int x, int y)
{
    // poisonedTiles.clear();//REMOVE ONLY THOSE ..
    removePoisonedTilesAround(x, y);
    updatePoisonedTiles();
    if(checkAllEnemiesDefeated()){
        emit i_gameOver();
        return;
    }
    // emit poisonLevelUpdated(0);
    emit protagonistMoved(Item::Enemy);
}

void GameModel::usePathfinder(int destX, int destY)
{
    std::vector<Node> nodes = copyTilesToNodes();

    Comparator<Node> comp = [](const Node& a, const Node& b) {
        return a.f > b.f;
    };
    int currentTileNumber = protag->getYPos()*numCols + protag->getXPos();
    Tile * startTile = tiles[currentTileNumber].get();
    Tile * destinationTile = tiles[destY*numCols + destX].get();

    unsigned int width = numCols;

    PathFinder<Node, Tile> pf(nodes, startTile, destinationTile, comp, width, heuristicWeight);

    moves.clear();
    moves = pf.A_star();
}

void GameModel::doAutoplayMove()
{
    gameState = GameState::Autoplay;
    if(gameState == GameState::Autoplay){
        Enemy * nearestEnemy = findNearestEnemy(protag->getXPos(), protag->getYPos());
        if(nearestEnemy){
            if(protag->getHealth() > nearestEnemy->getValue()){
                usePathfinder(nearestEnemy->getXPos(), nearestEnemy->getYPos());
                return;
            }
            qDebug() << "Nearest enemy exists, but health isn't sufficient";
        }
        Tile * nearestHealthPack = findNearestHealthPack(protag->getXPos(), protag->getYPos());
        if(nearestHealthPack) {
            usePathfinder(nearestHealthPack->getXPos(), nearestHealthPack->getYPos());
            return;
        }
        if(nearestEnemy){
            usePathfinder(nearestEnemy->getXPos(), nearestEnemy->getYPos());
            return;
        }
        qDebug() << "No nearest healthpack found";
    }
    else{
        emit i_gameOver();
    }
}

std::vector<Node> GameModel::copyTilesToNodes()
{
    std::vector<Node> nodes;
    nodes.reserve(tiles.size());

    for (const auto& tile : tiles) {
        if (tile) { // Check if the pointer is not null
            nodes.emplace_back(*tile); // Copy the Tile to a new Node
        }
    }

    return nodes;
}
