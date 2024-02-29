#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "baseview.h"
#include "qpixmap.h"
#include "Model/gamemodel.h"

class GraphicsView : public BaseView
{
public:
    explicit GraphicsView(QGraphicsScene* scene) : BaseView(scene) {};
    void drawWorld() override; // Implement drawing the world graphically.
    virtual ~GraphicsView(){};

    // Implement specific update methods for graphics view.
    void updateEnemies() override;
    void updateHealthPacks() override;
    void updateProtagonist() override;
    void show() override;
    void hide() override;
    void updateTileColor() override;
    void showPoison() override;
    QGraphicsPixmapItem* getTileItem(int x, int y);


protected:
    QPixmap tilePixmap;
    QPixmap enemyPixmap;
    QPixmap protagonistPixmap;
    QPixmap healthPackPixmap;

    QGraphicsPixmapItem* protagonistItem;  // Pointer to the protagonist's pixmap item
    QMap<Enemy*, QGraphicsPixmapItem*> enemyItems; //References to enemies
    QMap<Enemy*, QGraphicsTextItem*> enemyValueLabels; // References to labels showing enemies' values
    QMap<Tile*, QGraphicsPixmapItem*> healthPackItems; //References to healthpacks
    QMap<Tile*, QGraphicsTextItem*> healthpackValueLabels; // References to labels showing enemies' values
    QMap<QPair<int, int>, QGraphicsPixmapItem*> tileItems; //References to pixmap items of the tiles

    //Private methods to help drawWorld
    void drawTile(QGraphicsScene* scene, const std::unique_ptr<Tile>& tile);
    void drawProtagonist(QGraphicsScene* scene, const std::unique_ptr<Protagonist>& protagonist);
    void drawEnemy(QGraphicsScene* scene, const std::unique_ptr<Enemy>& enemy);
    void drawHealthPack(QGraphicsScene* scene, const std::unique_ptr<Tile>& healthPack);
    void updateTile(const std::unique_ptr<Tile>& tile);
};

#endif // GRAPHICSVIEW_H
