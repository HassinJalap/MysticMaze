#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H

#include "View/graphicsview.h"

class OverlayView : public GraphicsView
{

public:
    explicit OverlayView(QGraphicsScene* scene) : GraphicsView(scene) {};
    ~OverlayView() override;

    void drawTile(const std::unique_ptr<Tile>& tile);
    void drawWorld() override;
    void showPoison() override;
    void updateTile(const std::unique_ptr<Tile>& tile);
};

#endif // OVERLAYVIEW_H
