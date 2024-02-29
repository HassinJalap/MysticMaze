#ifndef XENEMY_H
#define XENEMY_H

#include "world.h"
#include "behaviorstrategy.h"


class XEnemy : public Enemy {
private:
    BehaviorStrategy* behaviorStrategy;  // Change the member type

public:
    XEnemy(int xPosition, int yPosition, float strength);
    ~XEnemy();
    int xposition;
    int yposition;
    float strength;
    void setBehavior(BehaviorStrategy* strategy);
    void performBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies);
    BehaviorStrategy* getBehavior();
    void setXPosition(int xPosition);
    void setYPosition(int yPosition);
    void setValue(float Strength);
};

#endif // XENEMY_H
