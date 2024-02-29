#ifndef BASEVIEW_H
#define BASEVIEW_H
#include <QObject>
#include <QGraphicsScene>
#include "Model/gamemodel.h"

enum ViewType { Ascii = 1, Graphics = 2 };

class BaseView : public QObject
{
    Q_OBJECT
public:
    explicit BaseView(QGraphicsScene* scene) : scene(scene) {};

    virtual void drawWorld() = 0; //Update the view
    virtual void updateEnemies() = 0;
    virtual void updateHealthPacks() = 0;
    virtual void updateProtagonist() = 0;
    virtual void show() = 0; // make the items visible in the scene
    virtual void hide() = 0; // hide the items in the scene (to not delete them)
    virtual void updateTileColor() = 0;
    virtual void showPoison() = 0;
    void setModel(std::shared_ptr<GameModel> model){this->model = model;};

protected:
    std::shared_ptr<GameModel> model;
    QGraphicsScene* scene;

    void clearScene(QGraphicsScene* scene) {
        QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            if (item->type() == QGraphicsTextItem::Type) {
                scene->removeItem(item);
                delete item;
            }
        }
    };


};

#endif // BASEVIEW_H
