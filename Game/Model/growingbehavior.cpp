#include "growingbehavior.h"
#include <QDebug>

GrowingBehavior::GrowingBehavior(Enemy* enemy) : BehaviorStrategy(enemy), enemyPtr(enemy) {

}

GrowingBehavior::~GrowingBehavior() {

}

void GrowingBehavior::executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {
    moveEnemy(tileWidth, tileHeight, enemies);
}

void GrowingBehavior::moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) {

    auto foundEnemy = std::find_if(enemies.begin(), enemies.end(), [this](const std::unique_ptr<Enemy>& enemy) {
        Enemy* castedEnemyPtr = dynamic_cast<Enemy*>(enemyPtr);
        return castedEnemyPtr == enemy.get();
    });

    if (foundEnemy != enemies.end()) {
        // Enemy* foundEnemyPtr = foundEnemy->get();
        if(foundEnemy->get()){
            if(foundEnemy->get()->getValue()+1 <= 80){
                (*foundEnemy)->setValue(foundEnemy->get()->getValue()+5);
            }
            else if(foundEnemy->get()->getValue()+1 <= 100){
                (*foundEnemy)->setValue(foundEnemy->get()->getValue()+1);
            }
            size_t foundIndex = std::distance(enemies.begin(), foundEnemy);
            qDebug() << "Found enemy at index: " << foundIndex;
        }

    } else {
        qDebug() << "Enemy not found.";
    }

}
