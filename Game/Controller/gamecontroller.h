#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include "Model/gamemodel.h"
#include "View/baseview.h"
#include "View/asciiview.h"
#include "View/graphicsview.h"
#include "View/overlayview.h"

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(std::shared_ptr<GameModel> model, std::shared_ptr<AsciiView> asciiView,
                   std::shared_ptr<GraphicsView> graphicsView, std::shared_ptr<OverlayView> overlayView, QObject* parent = nullptr);
    void startGame(int enemies, int healthpacks, int xenemies);

 public slots:
    void showProtagonistMovement(Item item);
    void startAutoplay();
    void switchView();
    void handleMoveRequest(int xDir, int yDir);
    void handleAttackNearestEnemy();
    void handleGetNearestHealthpack();
    void handleGoToTile(int xPos, int yPos);
    void ProcessMoveTextCommand();
    void handleVisualizePoisonLevel();

    void handleEnergyChanged(int newEnergy){qDebug() << "Emitting energy change"; emit energyChanged(newEnergy);};
    void handleHealthChanged(int newHealth){qDebug() << "Emitting health change"; emit healthChanged(newHealth);};
    void showGameOver();
    void onHeuristicSliderChanged(int value);

signals:
    void moveProtagonist(int Ydir, int yDir);
    void protagonistMovedCenter(int xPos, int yPos);

    void gameOver(GameResult result);
    //To update energy on the progress bars in the UI
    void energyChanged(int newEnergy);
    void healthChanged(int newHealth);
    void moveTextCommand();

private:

    std::shared_ptr<GameModel> gameModel;
    std::shared_ptr<BaseView> currentView;
    std::shared_ptr<AsciiView> asciiView;
    std::shared_ptr<GraphicsView> graphicsView;
    std::shared_ptr<OverlayView> overlayView;
    QTimer* autoplayTimer;
    QTimer* textCommandTimer;
    // QTimer* autoplayPauseTimer;

    void processNextMove();
    void move(int move);
    int numberOfMoves;
    bool isTextCommand;
};

#endif // GAMECONTROLLER_H
