#include "gamecontroller.h"
#include <iostream>


GameController::GameController(std::shared_ptr<GameModel> model, std::shared_ptr<AsciiView> asciiView,
                               std::shared_ptr<GraphicsView> graphicsView, std::shared_ptr<OverlayView> overlayView,
                               QObject* parent)
    : QObject(parent), gameModel(model), asciiView(asciiView), graphicsView(graphicsView), overlayView(overlayView)
{
    currentView = graphicsView;
    asciiView->hide();
    graphicsView->hide();
    overlayView->hide();
    connect(this, &GameController::moveProtagonist, gameModel.get(), &GameModel::moveProtagonist);
    connect(gameModel.get(), &GameModel::protagonistMoved, this, &GameController::showProtagonistMovement);
    // Inside the startGame or similar function after the protagonist is created/initialized
    connect(gameModel.get(), &GameModel::energyChanged, this, &GameController::handleEnergyChanged);
    connect(gameModel.get(), &GameModel::healthChanged, this, &GameController::handleHealthChanged);
    connect(gameModel.get(), &GameModel::gameOver, this, &GameController::showGameOver);
    //Timer set up
    autoplayTimer = new QTimer(this);
    textCommandTimer = new QTimer(this);
    connect(textCommandTimer, &QTimer::timeout, this, &GameController::ProcessMoveTextCommand);
    textCommandTimer->setInterval(50);
    connect(autoplayTimer, &QTimer::timeout, this, &GameController::processNextMove);
    autoplayTimer->setInterval(50);
    //Poison

}

void GameController::startGame(int enemies, int healthpacks, int xenemies)
{
    gameModel->startGame(enemies, healthpacks, xenemies);
    graphicsView->drawWorld();
    asciiView->drawWorld();
    asciiView->hide();
    currentView = graphicsView;
    currentView->drawWorld();
    currentView->show();
    emit protagonistMovedCenter(0, 0);
    connect(gameModel.get(), &GameModel::updatePoisonedTiles, this, &GameController::handleVisualizePoisonLevel);


}

void GameController::showProtagonistMovement(Item item){
    emit protagonistMovedCenter(gameModel->getProtag()->getXPos(), gameModel->getProtag()->getYPos());
    switch(item) {
    case Item::Protagonist:
        currentView->updateProtagonist();
        qDebug() << "In showProtagonistMovement! Just moving the protagonist";
        if(gameModel->getGameState() == GameState::Autoplay || isTextCommand){
            currentView->updateTileColor();
        }
        break;
    case Item::Healthpack:
        currentView->updateHealthPacks();
        currentView->updateProtagonist();
        if(gameModel->getGameState() == GameState::Autoplay || isTextCommand){
            currentView->updateTileColor();
        }
        qDebug() << "In showProtagonistMovement on a healthpack!";
        break;
    case Item::Enemy:
        currentView->updateEnemies();
        currentView->updateProtagonist();
        // currentView->drawWorld();
        if(gameModel->getGameState() == GameState::Autoplay || isTextCommand){
            currentView->updateTileColor();
        }
        qDebug() << "In showProtagonistMovement on an enemy!";
        break;

    case Item::Door:
        std::cout << "On a door!";
        break;
    }
}

void GameController::startAutoplay()
{
    if(gameModel->getGameState() == GameState::Running || gameModel->getGameState() == GameState::Autoplay){
        gameModel->doAutoplayMove();
        if(!gameModel->getMoves().empty()){
            qDebug() << "moves:" << gameModel->getMoves();
            numberOfMoves = gameModel->getMoves().size();
            autoplayTimer->start();
        }
    }

}

void GameController::move(int move){
    switch(move) {
    case(0): emit moveProtagonist(0, -1); break;
    case(1): emit moveProtagonist(-1, -1); break;
    case(2): emit moveProtagonist(-1, 0); break;
    case(3): emit moveProtagonist(-1, 1); break;
    case(4): emit moveProtagonist(0, 1); break;
    case(5): emit moveProtagonist(1, 1); break;
    case(6): emit moveProtagonist(1, 0); break;
    case(7): emit moveProtagonist(1, -1); break;
    }
}

void GameController::processNextMove(){
    if(numberOfMoves > 0){
        int current_move = gameModel->getMoves()[gameModel->getMoves().size() - numberOfMoves];
        move(current_move);
        numberOfMoves -= 1;
    }
    else{
        autoplayTimer->stop(); // Stop the timer if there are no more moves
        if(gameModel->getGameState() == GameState::Autoplay){
            qDebug() << "GS rn is: Autoplay!";
            QTimer::singleShot(1000, this, &GameController::startAutoplay);
        }
        else{
            emit gameOver(gameModel->getGameResult());
        }
    }
}

void GameController::handleMoveRequest(int xDir, int yDir) {
    qDebug() << "In game controller handleMoveRequest";
    emit moveProtagonist(xDir, yDir);
}

void GameController::handleAttackNearestEnemy()
{
    if(gameModel->getGameState() == GameState::Running){
        if(auto it = gameModel->findNearestEnemy(gameModel->getProtag()->getXPos(), gameModel->getProtag()->getYPos())){
            gameModel->usePathfinder(it->getXPos(), it->getYPos());
            if(!gameModel->getMoves().empty()){
                qDebug() << "moves:" << gameModel->getMoves();
                numberOfMoves = gameModel->getMoves().size();
                textCommandTimer->start();
            }
        }
    }

}

void GameController::handleGetNearestHealthpack()
{
    if(gameModel->getGameState() == GameState::Running){
        if(auto it = gameModel->findNearestHealthPack(gameModel->getProtag()->getXPos(), gameModel->getProtag()->getYPos())){
            gameModel->usePathfinder(it->getXPos(), it->getYPos());
            if(!gameModel->getMoves().empty()){
                qDebug() << "moves:" << gameModel->getMoves();
                numberOfMoves = gameModel->getMoves().size();
                textCommandTimer->start();
            }
        }
    }
}

void GameController::handleGoToTile(int xPos, int yPos)
{
    qDebug() << "I'm in handleGoToTile";
    if(gameModel->getGameState() == GameState::Running){
        gameModel->usePathfinder(xPos, yPos);
        if(!gameModel->getMoves().empty()){
            qDebug() << "moves:" << gameModel->getMoves();
            numberOfMoves = gameModel->getMoves().size();
            isTextCommand = true;
            textCommandTimer->start();
        }
    }

}

void GameController::ProcessMoveTextCommand()
{
    if(numberOfMoves > 0){
        int current_move = gameModel->getMoves()[gameModel->getMoves().size() - numberOfMoves];
        move(current_move);
        numberOfMoves -= 1;
    }
    else{
        isTextCommand = false;
        textCommandTimer->stop(); // Stop the timer if there are no more moves
    }
}

void GameController::handleVisualizePoisonLevel()
{
    currentView->showPoison();
}

void GameController::switchView()
{
    if (currentView == graphicsView) {
        currentView = asciiView;
        currentView->drawWorld();
        graphicsView->hide();
        overlayView->hide();
        asciiView->show();
    } else if(currentView == asciiView) {
        currentView = overlayView;
        currentView->drawWorld();
        graphicsView->hide();
        asciiView->hide();
        overlayView->show();
    }
    else{
        currentView = graphicsView;
        currentView->drawWorld();
        asciiView->hide();
        overlayView->hide();
        graphicsView->show();
    }

}

void GameController::showGameOver(){
    disconnect(gameModel.get(), &GameModel::updatePoisonedTiles, this, &GameController::handleVisualizePoisonLevel);
    emit gameOver(gameModel->getGameResult());
}

void GameController::onHeuristicSliderChanged(int value)
{
    float heuristicWeight = static_cast<float>(value) / 100.0f;
    gameModel->setHeuristicWeight(heuristicWeight);
}
