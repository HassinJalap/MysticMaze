#ifndef MOVINGBEHAVIOR_H
#define MOVINGBEHAVIOR_H

#include "behaviorstrategy.h"

class MovingBehavior : public BehaviorStrategy {
public:
    MovingBehavior(Enemy* enemy);  // Change the parameter type
    ~MovingBehavior() override;

    void executeBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies) override;
    void moveEnemy(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies);

private:
    Enemy* enemyPtr;
    float enemyValue;
    int enemyXPos;
};


#endif // MOVINGBEHAVIOR_H


