#include "overlayview.h"


OverlayView::~OverlayView(){}

void OverlayView::drawTile(const std::unique_ptr<Tile> &tile)
{
    int x = tile->getXPos();
    int y = tile->getYPos();
    QGraphicsPixmapItem *pixmapItem;

    QString imagePath;
    if (std::isinf(tile->getValue())) {
        imagePath = ":/Images/brick.jpg"; // Image for infinite value (brick)
    } else if (tile->getValue() > 0.6) {
        imagePath = ":/Images/rough_tile.png"; // Image for rough terrain
    } else if (tile->getValue() > 0.25) {
        imagePath = ":/Images/sandy_tile.jpg"; // Image for medium terrain
    } else {
        imagePath = ":/Images/Smooth_tile1.png"; // Image for smooth terrain
    }
    QPixmap tilePixmap(imagePath);
    pixmapItem = new QGraphicsPixmapItem(tilePixmap.scaled(model->getTileWidth(), model->getTileHeight()));
    pixmapItem->setPos(x * model->getTileWidth(), y * model->getTileHeight());
    tileItems[{tile->getXPos(), tile->getYPos()}] = pixmapItem; // Storing reference to the item
    pixmapItem->setData(0, QVariant(Graphics));  // Tag the item
    scene->addItem(pixmapItem);


    // QColor color = QColor::fromRgbF(0, grayscaleValue, 0);
    // QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
    // coloredTilePixmap.fill(color);
    // pixmapItem = new QGraphicsPixmapItem(coloredTilePixmap);
    // pixmapItem->setPos(x * model->getTileWidth(), y * model->getTileHeight());
    // tileItems[{tile->getXPos(), tile->getYPos()}] = pixmapItem; //Storing reference to the item
    // pixmapItem->setData(0, QVariant(Graphics));  // Tag the item
    // scene->addItem(pixmapItem);
}

void OverlayView::drawWorld()
{
    clearScene(scene);

    // Draw tiles
    for (const auto& tile : model->getTiles()) {
        drawTile(tile);
    }
    showPoison();

    // Draw protagonist
    drawProtagonist(scene, model->getProtag());

    enemyItems.clear();
    enemyValueLabels.clear();
    // Draw enemies
    for (const auto& enemy : model->getEnemies()) {
        drawEnemy(scene, enemy);
    }

    healthpackValueLabels.clear();
    healthPackItems.clear();
    // Draw health packs
    for (const auto& healthPack : model->getHealthPacks()) {
        drawHealthPack(scene, healthPack);
    }
}

void OverlayView::showPoison()
{
    // Update tile colors
    for (const auto& tile : model->getTiles()) {
        updateTile(tile);
    }

    // Get the list of poisoned tile positions from the model
    const auto& poisonedTiles = model->getPoisonedTiles();

    // Iterate through the poisoned tiles and update their color
    for (const auto& poisonedTilePos : poisonedTiles) {
        // Find the corresponding tile item in the view
        QGraphicsPixmapItem* tileItem = getTileItem(poisonedTilePos->getXPos(), poisonedTilePos->getYPos());
        if (tileItem) {
            // Create a new pixmap with the updated color and set it to the tile item
            QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
            coloredTilePixmap.fill(QColor::fromRgb(255, 255, 0));
            tileItem->setPixmap(coloredTilePixmap);
        }
    }
}

void OverlayView::updateTile(const std::unique_ptr<Tile> &tile)
{
    // drawTile(tile);
    QGraphicsPixmapItem *pixmapItem = getTileItem(tile->getXPos(), tile->getYPos());
    if(pixmapItem){
        float grayscaleValue = tile->getValue();

        QString imagePath;
        if (std::isinf(grayscaleValue)) {
            imagePath = ":/Images/brick.jpg"; // Image for infinite value (brick)
        } else if (grayscaleValue > 0.6) {
            imagePath = ":/Images/rough_tile.png"; // Image for rough terrain
        } else if (grayscaleValue > 0.25) {
            imagePath = ":/Images/sandy_tile.jpg"; // Image for medium terrain
        } else {
            imagePath = ":/Images/Smooth_tile1.png"; // Image for smooth terrain
        }
        QPixmap tilePixmap(imagePath);
        pixmapItem->setPixmap(tilePixmap);


        // QColor color = QColor::fromRgbF(0, grayscaleValue, 0);
        // QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
        // coloredTilePixmap.fill(color);
        // pixmapItem->setPixmap(coloredTilePixmap);
    }
}
