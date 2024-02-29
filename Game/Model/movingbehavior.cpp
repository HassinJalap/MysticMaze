#include "movingbehavior.h"
#include <QDebug>

MovingBehavior::MovingBehavior(Enemy* enemy) : BehaviorStrategy(enemy), enemyPtr(enemy) {

}

MovingBehavior::~MovingBehavior() {

}

void MovingBehavior::executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {
    moveEnemy(tileWidth, tileHeight, enemies);
}

void MovingBehavior::moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {

    int randomXChange = rand() % 3 - 1;  // -1, 0, or 1
    int randomYChange = rand() % 3 - 1;  // -1, 0, or 1

    int newXPos = enemyPtr->getXPos() + randomXChange;
    int newYPos = enemyPtr->getYPos() + randomYChange;

    newXPos = std::max(0, std::min(tileWidth - 1, newXPos));
    newYPos = std::max(0, std::min(tileHeight - 1, newYPos));

    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [this](const std::unique_ptr<Enemy>& enemy) {
        Enemy* castedEnemyPtr = dynamic_cast<Enemy*>(enemyPtr);
        return castedEnemyPtr == enemy.get();
    });

    if (foundEnemy != enemies.end()) {
        // Enemy* foundEnemyPtr = foundEnemy->get();
        if(foundEnemy->get()){
            (*foundEnemy)->setXPos(newXPos);
            (*foundEnemy)->setYPos(newYPos);
            size_t foundIndex = std::distance(enemies.begin(), foundEnemy);
            qDebug() << "Found enemy at index: " << foundIndex;
        }

    } else {
        qDebug() << "Enemy not found.";
    }

}
