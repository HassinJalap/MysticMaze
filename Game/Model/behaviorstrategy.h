#ifndef BEHAVIORSTRATEGY_H
#define BEHAVIORSTRATEGY_H

#include "world.h"

class BehaviorStrategy {
public:
    BehaviorStrategy(Enemy* enemy) : enemyPtr(enemy){};  // Change the parameter type
    virtual ~BehaviorStrategy(){};

    virtual void executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) = 0;

protected:
    Enemy* enemyPtr;  // Change the member type
};

#endif // BEHAVIORSTRATEGY_H
