#include "graphicsview.h"
#include "Model/xenemy.h"



void GraphicsView::drawWorld()
{
    clearScene(scene);

    // Draw tiles
    for (const auto& tile : model->getTiles()) {
        drawTile(scene, tile);
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

void GraphicsView::drawTile(QGraphicsScene* scene, const std::unique_ptr<Tile>& tile) {
    int x = tile->getXPos();
    int y = tile->getYPos();
    QGraphicsPixmapItem *pixmapItem;
    float grayscaleValue;
        if (std::isfinite(tile->getValue())) {
            grayscaleValue = tile->getValue();
        }
        else{
            grayscaleValue = 255;
        }
        QColor color = QColor::fromRgbF(0, grayscaleValue, 0);
        QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
        coloredTilePixmap.fill(color);
        pixmapItem = new QGraphicsPixmapItem(coloredTilePixmap);
        pixmapItem->setPos(x * model->getTileWidth(), y * model->getTileHeight());
        tileItems[{tile->getXPos(), tile->getYPos()}] = pixmapItem; //Storing reference to the item
        pixmapItem->setData(0, QVariant(Graphics));  // Tag the item
        scene->addItem(pixmapItem);
}

QGraphicsPixmapItem* GraphicsView::getTileItem(int x, int y) {
    auto key = QPair<int, int>(x, y);
    if (tileItems.contains(key)) {
        return tileItems[key];
    }
    return nullptr; // or handle this case as needed
}

void GraphicsView::updateTileColor() { //this is for autoplay. Rename it better
    int x = model->getProtag()->getXPos();
    int y = model->getProtag()->getYPos();
    QGraphicsPixmapItem* tileItem = getTileItem(x, y);
    if (tileItem) {
        QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
        coloredTilePixmap.fill(QColor::fromRgb(255,0,0));
        tileItem->setPixmap(coloredTilePixmap);
    }
}

void GraphicsView::updateTile(const std::unique_ptr<Tile>& tile){
    QGraphicsPixmapItem *pixmapItem = getTileItem(tile->getXPos(), tile->getYPos());
    if(pixmapItem){
        float grayscaleValue = tile->getValue();
        QColor color = QColor::fromRgbF(0, grayscaleValue, 0);
        QPixmap coloredTilePixmap(model->getTileWidth(), model->getTileHeight());
        coloredTilePixmap.fill(color);
        pixmapItem->setPixmap(coloredTilePixmap);
    }
}

void GraphicsView::showPoison()
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

void GraphicsView::drawProtagonist(QGraphicsScene* scene, const std::unique_ptr<Protagonist>& protagonist) {
    // Use a pixmap to represent the protagonist
    QPixmap protagonistPixmap(":/Images/Mario.jpg");
    protagonistItem = new QGraphicsPixmapItem(protagonistPixmap);
    protagonistItem->setPos(protagonist->getXPos() * model->getTileWidth(), protagonist->getYPos() * model->getTileHeight());
    float healthRatio = model->getProtag()->getHealth() / 100.0; // Assuming max health is 100
    protagonistItem->setOpacity(healthRatio);

    protagonistItem->setData(0, QVariant(Graphics));  // Tag the item

    scene->addItem(protagonistItem);
}

void GraphicsView::drawEnemy(QGraphicsScene* scene, const std::unique_ptr<Enemy>& enemy) {
    if(!enemy->getDefeated()){
        // Use a pixmap to represent the enemy
        QPixmap enemyPixmap;
        if(XEnemy* xEnemy = dynamic_cast<XEnemy*>(enemy.get())){
            qDebug() << "Drawing xEnemy";
            enemyPixmap.load(":/Images/xenemy.png");
        }
        else{
            enemyPixmap.load(":/Images/enemy.png");
        }

        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(enemyPixmap);
        pixmapItem->setPos(enemy->getXPos() * model->getTileWidth(), enemy->getYPos() * model->getTileHeight());

        enemyItems[enemy.get()] = pixmapItem;  // Store the item reference
        pixmapItem->setData(0, QVariant(Graphics));  // Tag the item
        scene->addItem(pixmapItem);

        // Create a label for the enemy's value
        QGraphicsTextItem* valueLabel = new QGraphicsTextItem();
        valueLabel->setPos(enemy->getXPos()*model->getTileWidth(), enemy->getYPos()*model->getTileHeight()-20); // Adjust this offset as needed
        valueLabel->setPlainText(QString::number(enemy->getValue()));
        valueLabel->setDefaultTextColor(Qt::blue); // Set the color as needed
        valueLabel->setData(0, QVariant(Graphics));  // Tag the item
        enemyValueLabels[enemy.get()] = valueLabel;  // Store the item reference
        scene->addItem(valueLabel);
        qDebug() << "Set the label to scene";
    }

}

void GraphicsView::drawHealthPack(QGraphicsScene* scene, const std::unique_ptr<Tile>& healthPack) {
    // Use a pixmap to represent the health pack
    QPixmap healthPackPixmap(":/Images/healthPack.png");
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(healthPackPixmap);
    pixmapItem->setPos(healthPack->getXPos() * model->getTileWidth(), healthPack->getYPos() * model->getTileHeight());

    healthPackItems[healthPack.get()] = pixmapItem;  // Store the item reference
    pixmapItem->setData(0, QVariant(Graphics));  // Tag the item
    scene->addItem(pixmapItem);

    // Create a label for the healthpacks's value
    QGraphicsTextItem* valueLabel = new QGraphicsTextItem();
    healthpackValueLabels[healthPack.get()] = valueLabel;
    valueLabel->setPos(healthPack->getXPos()*model->getTileWidth(), healthPack->getYPos()*model->getTileHeight() -20); // Adjust this offset as needed
    valueLabel->setPlainText(QString::number(healthPack->getValue()));
    valueLabel->setDefaultTextColor(Qt::blue); // Set the color as needed
    scene->addItem(valueLabel);
}

void GraphicsView::updateEnemies() {
    for (const auto& enemy : model->getEnemies()) {
        if (enemy->getDefeated()) {
            auto pixmapIt = enemyItems.find(enemy.get());
            if (pixmapIt != enemyItems.end()) {
                QGraphicsPixmapItem* item = pixmapIt.value();
                scene->removeItem(item);  // Remove the item from the scene
                delete item;  // Delete the item to free memory
                enemyItems.erase(pixmapIt);  // Remove the entry from the map
            }
            auto pixmapLabelIt = enemyValueLabels.find(enemy.get());
            if (pixmapLabelIt != enemyValueLabels.end()) {
                QGraphicsTextItem* item = pixmapLabelIt.value();
                scene->removeItem(item);  // Remove the item from the scene
                delete item;  // Delete the item to free memory
                enemyValueLabels.erase(pixmapLabelIt);  // Remove the entry from the map
            }
        }
        else{
            auto enemyPixmap = enemyItems.find(enemy.get());
            if(enemyPixmap != enemyItems.end()){
                enemyPixmap.value()->setPos(enemy->getXPos() * model->getTileWidth(), enemy->getYPos() * model->getTileHeight());
            }

            auto enemyValuePixmap = enemyValueLabels.find(enemy.get());
            if(enemyValuePixmap != enemyValueLabels.end()){
                enemyValuePixmap.value()->setPlainText(QString::number(enemy->getValue()));
                enemyValuePixmap.value()->setPos(enemy->getXPos()*model->getTileWidth(), enemy->getYPos()*model->getTileHeight()-20);
            }

        }
    }
}

void GraphicsView::updateHealthPacks()
{
    // Assume model->getHealthPacks() returns a list or similar collection of current health packs.
    const auto &currentHealthPacks = model->getHealthPacks();

    for (auto it = healthPackItems.begin(); it != healthPackItems.end();) {
        bool stillExists = false;

        // Check if the health pack still exists in the model
        for (const auto& modelHealthPack : currentHealthPacks) {
            if (it.key() == modelHealthPack.get()) {
                stillExists = true;
                break;
            }
        }

        if (!stillExists) {
            it.value()->hide();  // Hide the item representing the consumed health pack
            it = healthPackItems.erase(it);  // Remove from the map
        } else {
            ++it;
        }
    }

    for (auto it = healthpackValueLabels.begin(); it != healthpackValueLabels.end();) {
        bool stillExists = false;

        // Check if the health pack still exists in the model
        for (const auto& modelHealthPack : currentHealthPacks) {
            if (it.key() == modelHealthPack.get()) {
                stillExists = true;
                break;
            }
        }

        if (!stillExists) {
            it.value()->hide();  // Hide the item representing the consumed health pack
            it = healthpackValueLabels.erase(it);  // Remove from the map
        } else {
            ++it;
        }
    }
}

void GraphicsView::updateProtagonist()
{
    if (protagonistItem) {
        protagonistItem->setPos(model->getProtag()->getXPos() * model->getTileWidth(),
                                model->getProtag()->getYPos() * model->getTileHeight());
        float healthRatio = model->getProtag()->getHealth() / 100.0; // Assuming max health is 100
        protagonistItem->setOpacity(healthRatio);
    }
}

void GraphicsView::show() {
    // Iterate through all items in the scene
    for (auto& item : scene->items()) {
        // Check if the item has the Graphics tag
        if (item->data(0).toInt() == Graphics) {
            item->setVisible(true);  // Make the item visible
        }
    }
}

void GraphicsView::hide() {
    // Iterate through all items in the scene
    for (auto& item : scene->items()) {
        // Check if the item has the Graphics tag
        if (item->data(0).toInt() == Graphics) {
            item->setVisible(false);  // Make the item invisible
        }
    }
}
