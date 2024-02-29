#ifndef ASCIIVIEW_H
#define ASCIIVIEW_H

#include "baseview.h"
#include <QGraphicsScene>

class AsciiView : public BaseView
{
public:
    explicit AsciiView(QGraphicsScene* scene) : BaseView(scene) {};

    void drawWorld() override; // Implement drawing the world graphically.

    // Implement specific update methods for graphics view.
    void updateEnemies() override;
    void updateHealthPacks() override;
    void updateProtagonist() override;
    void show() override;
    void hide() override;
    void updateTileColor() override;
    void showPoison() override;

private:
    void clearEnemyLabels();
    void drawTile(QGraphicsScene* scene, const std::unique_ptr<Tile>& tile);
    void drawProtagonist(QGraphicsScene* scene, const std::unique_ptr<Protagonist>& protagonist);
    void drawEnemy(QGraphicsScene* scene, const std::unique_ptr<Enemy>& enemy);
    void drawHealthPack(QGraphicsScene* scene, const std::unique_ptr<Tile>& healthPack);

    //helper method to set protagonist color based on health value
    void setProtagColor();

    // QGraphicsTextItem* protagonistLabel;
    // QMap<Enemy*, QSharedPointer<QGraphicsTextItem>> enemyLabels; //References to enemies
    // QMap<Enemy*, QSharedPointer<QGraphicsTextItem>> enemyValueLabels; //References to enemy value labels
    // QMap<Tile*, QSharedPointer<QGraphicsTextItem>> healthPackLabels; //References to healthpacks
    // QMap<Tile*, QSharedPointer<QGraphicsTextItem>> healthPackValueLabels; //References to healthpack labels

    QGraphicsTextItem* protagonistLabel;
    QMap<Enemy*, QGraphicsTextItem*> enemyLabels; //References to enemies
    QMap<Enemy*, QGraphicsTextItem*> enemyValueLabels; //References to enemies
    QMap<Tile*, QGraphicsTextItem*> healthPackLabels; //References to healthpacks
    QMap<Tile*, QGraphicsTextItem*> healthPackValueLabels; //References to healthpacks
    QMap<Tile*, QGraphicsTextItem*> poisonLabels;

};

#endif // ASCIIVIEW_H
