#ifndef TELEPORTBEHAVIOR_H
#define TELEPORTBEHAVIOR_H

#include "behaviorstrategy.h"

class TeleportBehavior : public BehaviorStrategy {
public:
    TeleportBehavior(Enemy* enemy);  // Change the parameter type
    ~TeleportBehavior() override;

    void executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) override;
    void moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies);

private:
    Enemy* enemyPtr;
    float enemyValue;
    int enemyXPos;
};

#endif // TELEPORTBEHAVIOR_H



