#include "xenemy.h"

XEnemy::XEnemy(int xPosition, int yPosition, float strength) : Enemy(xPosition, yPosition, strength), behaviorStrategy(nullptr)
{
    this->xposition = xPosition;
    this->yposition = yPosition;
    this->strength = strength;
    //qDebug() << "xposition:" << this->xposition <<"yposition: "<< this->yposition <<"value: "<<this->strength ;
}


XEnemy::~XEnemy()
{
    delete behaviorStrategy;
}

void XEnemy::setXPosition(int xPosition){
    xposition = xPosition;
}

void XEnemy::setYPosition(int yPosition){
    yposition = yPosition;
}

void XEnemy::setValue(float Strength){
    strength = Strength;
}

void XEnemy::setBehavior(BehaviorStrategy* strategy )
{
    delete behaviorStrategy;
    behaviorStrategy = strategy;
}

void XEnemy::performBehavior(int tileWidth, int tileHeight, std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (behaviorStrategy)
    {
        behaviorStrategy->executeBehavior(tileWidth, tileHeight, enemies);
    }
}

BehaviorStrategy* XEnemy::getBehavior()
{
    return behaviorStrategy;
}
