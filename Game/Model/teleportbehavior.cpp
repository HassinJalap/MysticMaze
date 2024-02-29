#include "teleportbehavior.h"
#include <QDebug>

TeleportBehavior::TeleportBehavior(Enemy* enemy) : BehaviorStrategy(enemy), enemyPtr(enemy) {

}

TeleportBehavior::~TeleportBehavior() {

}

void TeleportBehavior::executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {
    moveEnemy(tileWidth, tileHeight, enemies);
}

void TeleportBehavior::moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {
    int randomX = rand() % tileWidth;
    int randomY = rand() % tileHeight;

    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [this](const std::unique_ptr<Enemy>& enemy) {
        Enemy* castedEnemyPtr = dynamic_cast<Enemy*>(enemyPtr);
        return castedEnemyPtr == enemy.get();
    });

    if (foundEnemy != enemies.end()) {
        // Enemy* foundEnemyPtr = foundEnemy->get();
        if(foundEnemy->get()){
            (*foundEnemy)->setXPos(randomX);
            (*foundEnemy)->setYPos(randomY);

            size_t foundIndex = std::distance(enemies.begin(), foundEnemy);
            qDebug() << "Found enemy at index: " << foundIndex;
        }

    } else {
        qDebug() << "Enemy not found.";
    }

}
