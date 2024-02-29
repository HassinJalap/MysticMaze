#ifndef GROWINGBEHAVIOR_H
#define GROWINGBEHAVIOR_H

#include "behaviorstrategy.h"

class GrowingBehavior : public BehaviorStrategy {
public:
    GrowingBehavior(Enemy* enemy);  // Change the parameter type
    ~GrowingBehavior() override;

    void executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) override;
    void moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies);

private:
    Enemy* enemyPtr;
    float enemyValue;
    int enemyXPos;
};

#endif // GROWINGBEHAVIOR_H




