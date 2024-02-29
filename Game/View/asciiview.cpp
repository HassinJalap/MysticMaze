#include "asciiview.h"
#include "Model/xenemy.h"


void AsciiView::drawWorld()
{
    clearScene(scene);
    // Draw tiles
    for (const auto& tile : model->getTiles()) {
        drawTile(scene, tile);
    }
    showPoison();

    // Draw protagonist
    drawProtagonist(scene, model->getProtag());

    // enemyLabels.clear();
    // enemyValueLabels.clear();
    for (const auto& enemy : model->getEnemies()) {
        drawEnemy(scene, enemy);
    }

    // Draw health packs
    healthPackLabels.clear();
    healthPackValueLabels.clear();
    for (const auto& healthPack : model->getHealthPacks()) {
        drawHealthPack(scene, healthPack);
    }

}

void AsciiView::drawTile(QGraphicsScene* scene, const std::unique_ptr<Tile>& tile) {
    if (std::isfinite(tile->getValue())) {
        QGraphicsTextItem *asciiLabel = new QGraphicsTextItem(" +- - - \n  | ");
        asciiLabel->setPos(tile->getXPos() * model->getTileWidth(), tile->getYPos() * model->getTileHeight());

        asciiLabel->setData(0, QVariant(Ascii)); //Tag the ascii items
        scene->addItem(asciiLabel);
        // scene->update();
    }
}

void AsciiView::drawProtagonist(QGraphicsScene* scene, const std::unique_ptr<Protagonist>& protagonist) {
    protagonistLabel = new QGraphicsTextItem(" P ");
    protagonistLabel->setPos((protagonist->getXPos() + 0.5) * model->getTileWidth(), (0.67 + protagonist->getYPos()) * model->getTileHeight());
    setProtagColor();
    protagonistLabel->setDefaultTextColor(Qt::blue);

    protagonistLabel->setData(0, QVariant(Ascii)); //Tag the ascii items
    scene->addItem(protagonistLabel);
}

void AsciiView::drawEnemy(QGraphicsScene* scene, const std::unique_ptr<Enemy>& enemy) {
    if(!enemy->getDefeated()){
        QGraphicsTextItem *asciiLabel;
        if(XEnemy* xEnemy = dynamic_cast<XEnemy*>(enemy.get())){
            asciiLabel = new QGraphicsTextItem(" X ");
            asciiLabel->setDefaultTextColor(Qt::red);
        }
        else{
            asciiLabel = new QGraphicsTextItem(" E ");
            asciiLabel->setDefaultTextColor(QColorConstants::Svg::brown);
        }
        asciiLabel->setPos((0.5 + enemy->getXPos()) * model->getTileWidth(), (0.67 + enemy->getYPos()) * model->getTileHeight());

        enemyLabels[enemy.get()] = asciiLabel;
        asciiLabel->setData(0, QVariant(Ascii)); //Tag the ascii items
        scene->addItem(asciiLabel);

        // Create a label for the enemy's value
        QGraphicsTextItem* valueLabel = new QGraphicsTextItem();
        valueLabel->setPos((0.5+enemy->getXPos())*model->getTileWidth(), enemy->getYPos()*model->getTileHeight());
        valueLabel->setPlainText(QString::number(enemy->getValue()));
        valueLabel->setDefaultTextColor(Qt::blue);
        enemyValueLabels[enemy.get()] = valueLabel;
        valueLabel->setData(0, QVariant(Ascii));
        scene->addItem(valueLabel);
    }

}

void AsciiView::drawHealthPack(QGraphicsScene* scene, const std::unique_ptr<Tile>& healthPack) {
    QGraphicsTextItem *asciiLabel = new QGraphicsTextItem(" H ");
    asciiLabel->setPos((0.5 + healthPack->getXPos()) * model->getTileWidth(), (0.67 + healthPack->getYPos()) * model->getTileHeight());
    asciiLabel->setDefaultTextColor(Qt::green);

    healthPackLabels[healthPack.get()] = asciiLabel;
    asciiLabel->setData(0, QVariant(Ascii)); //Tag for ascii items :)
    scene->addItem(asciiLabel);

    // Create a label for the healthpacks's value
    QGraphicsTextItem* valueLabel = new QGraphicsTextItem();
    healthPackValueLabels[healthPack.get()] = valueLabel;
    valueLabel->setPos((0.5+healthPack->getXPos())*model->getTileWidth(), healthPack->getYPos()*model->getTileHeight());
    valueLabel->setPlainText(QString::number(healthPack->getValue()));
    valueLabel->setDefaultTextColor(Qt::blue);
    scene->addItem(valueLabel);
}


void AsciiView::updateEnemies()
{
    for (const auto& enemy : model->getEnemies()) {
        if (enemy->getDefeated()) {
            auto it = enemyLabels.find(enemy.get());
            if (it != enemyLabels.end()) {
                QGraphicsTextItem* item = it.value();
                if(item && scene->items().contains(item)) {
                    scene->removeItem(item);
                    delete item;
                    enemyLabels.erase(it);
                }
            }

            auto labelIt = enemyValueLabels.find(enemy.get());
            if (labelIt != enemyValueLabels.end()) {
                QGraphicsTextItem* item = labelIt.value();
                if(item && scene->items().contains(item)) {
                    scene->removeItem(item);
                    delete item;
                    enemyValueLabels.erase(labelIt);
                }
            }

        }
        else{ //Needed for Xenemy
            auto enemyPixmap = enemyLabels.find(enemy.get());
            if(enemyPixmap != enemyLabels.end()){
                enemyPixmap.value()->setPos((0.5 + enemy->getXPos()) * model->getTileWidth(), (0.67 + enemy->getYPos()) * model->getTileHeight());
            }

            auto enemyValuePixmap = enemyValueLabels.find(enemy.get());
            if(enemyValuePixmap != enemyValueLabels.end()){
                enemyValuePixmap.value()->setPlainText(QString::number(enemy->getValue()));
                enemyValuePixmap.value()->setPos((0.5+enemy->getXPos())*model->getTileWidth(), enemy->getYPos()*model->getTileHeight());
            }
        }
    }
}

void AsciiView::updateHealthPacks()
{
    const auto &currentHealthPacks = model->getHealthPacks();

    for (auto it = healthPackLabels.begin(); it != healthPackLabels.end();) {
        bool stillExists = false;

        // Check if the health pack still exists in the model
        for (const auto& modelHealthPack : currentHealthPacks) {
            if (it.key() == modelHealthPack.get()) {
                stillExists = true;
                break;
            }
        }

        if (!stillExists) {
            if(it.value() && scene->items().contains(it.value())){
                it.value()->hide();  // Hide the item representing the consumed health pack
                it = healthPackLabels.erase(it);  // Remove from the map
            }

        } else {
            ++it;
        }
    }

    for (auto it = healthPackValueLabels.begin(); it != healthPackValueLabels.end();) {
        bool stillExists = false;

        // Check if the health pack still exists in the model
        for (const auto& modelHealthPack : currentHealthPacks) {
            if (it.key() == modelHealthPack.get()) {
                stillExists = true;
                break;
            }
        }

        if (!stillExists) {
            if(it.value() && scene->items().contains(it.value())){
                it.value()->hide();  // Hide the item representing the consumed health pack
                it = healthPackValueLabels.erase(it);  // Remove from the map
            }

        } else {
            ++it;
        }
    }
}

void AsciiView::updateProtagonist()
{
    protagonistLabel->setPos((model->getProtag()->getXPos() + 0.5) * model->getTileWidth(),
                             (0.67 + model->getProtag()->getYPos()) * model->getTileHeight());
    setProtagColor();
}

void AsciiView::show() {
    // Iterate through all items in the scene
    for (auto& item : scene->items()) {
        // Check if the item has the Ascii tag
        if (item->data(0).toInt() == Ascii) {
            item->setVisible(true);  // Make the item visible
        }
    }
}

void AsciiView::hide() {
    // Iterate through all items in the scene
    for (auto& item : scene->items()) {
        // Check if the item has the Ascii tag
        if (item->data(0).toInt() == Ascii) {
            item->setVisible(false);  // Make the item invisible
        }
    }
}

void AsciiView::updateTileColor()
{

}

void AsciiView::showPoison()
{
    // Create a set of currently poisoned tile positions for easy lookup
    QSet<QPair<int, int>> poisonedPositions;
    for (const auto& tile : model->getPoisonedTiles()) {
        poisonedPositions.insert({tile->getXPos(), tile->getYPos()});
    }

    // Remove labels for tiles that are no longer poisoned
    auto it = poisonLabels.begin();
    while (it != poisonLabels.end()) {
        QGraphicsTextItem* label = it.value();
        if (label && scene->items().contains(label)) {
            int x = label->x() / model->getTileWidth();
            int y = label->y() / model->getTileHeight();

            if (!poisonedPositions.contains({x, y})) {
                scene->removeItem(label);
                delete label;
                it = poisonLabels.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it; // Skip if label is null or not in scene
        }
        // if(label){
        //     int x = label->x() / model->getTileWidth();
        //     int y = label->y() / model->getTileHeight();

        //     if (!poisonedPositions.contains({x, y})) {
        //         if(scene->items().contains(label)){
        //             scene->removeItem(label);
        //             delete label;
        //         }
        //         // if(label){
        //         //     delete label;
        //         // }
        //         it = poisonLabels.erase(it);
        //     } else {
        //         ++it;
        //     }
        // }

    }

    // Add labels for newly poisoned tiles
    for (const auto& tile : model->getPoisonedTiles()) {
        int x = tile->getXPos();
        int y = tile->getYPos();

        // Check if label already exists
        if (!poisonLabels.contains(tile.get())) {
            // Create a new label for the poisoned tile
            QGraphicsTextItem* poisonLabel = new QGraphicsTextItem("%");
            poisonLabel->setPos((0.5 + x) * model->getTileWidth(), (0.67 + y) * model->getTileHeight());
            poisonLabel->setDefaultTextColor(Qt::magenta); // Choose an appropriate color
            scene->addItem(poisonLabel);

            // Store the label in the map
            poisonLabels[tile.get()] = poisonLabel;
        }
    }

    // poisonLabels.clear();
    // // Iterate through the poisoned tiles
    // for (const auto& tile : model->getPoisonedTiles()) {
    //     qDebug() << "BURAIYIM";
    //     int x = tile->getXPos();
    //     int y = tile->getYPos();

    //     // Create a new label for the poisoned tile
    //     QGraphicsTextItem* poisonLabel = new QGraphicsTextItem("%");
    //     poisonLabel->setPos((0.5 + x) * model->getTileWidth(), (0.67 + y) * model->getTileHeight());
    //     poisonLabel->setDefaultTextColor(Qt::magenta); // Choose an appropriate color
    //     scene->addItem(poisonLabel);

    //     // Store the label in the map
    //     poisonLabels[tile.get()] = poisonLabel;
    // }


}

void AsciiView::setProtagColor(){
    if(model->getProtag()->getHealth() < 20){
        protagonistLabel->setDefaultTextColor(Qt::red);
    }
    else if(model->getProtag()->getHealth() < 40){
        protagonistLabel->setDefaultTextColor(Qt::yellow);
    }
    else if(model->getProtag()->getHealth() < 75){
        protagonistLabel->setDefaultTextColor(Qt::blue);
    }
    else{
        protagonistLabel->setDefaultTextColor(Qt::green);
    }



}
