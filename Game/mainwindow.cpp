#include "mainwindow.h"
#include "View/overlayview.h"
#include "ui_mainwindow.h"
#include <iostream>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->pushButton->setText("Switch View");

    // Initialize the model and views
    std::shared_ptr<GameModel> model = std::make_shared<GameModel>();
    asciiView = std::make_shared<AsciiView>(scene);
    graphicsView = std::make_shared<GraphicsView>(scene);
    overlayView = std::make_shared<OverlayView>(scene);
    asciiView->setModel(model);
    graphicsView->setModel(model);
    overlayView->setModel(model);


    // Initialize the controller with the model and views
    controller = new GameController(model, asciiView, graphicsView, overlayView, this);

    hideForStart();

    //For the slider
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(100);
    ui->horizontalSlider->setValue(50);
    connect(ui->horizontalSlider, &QSlider::valueChanged, controller, &GameController::onHeuristicSliderChanged);

    // Connect the start/restart button to start the game
    connect(ui->startRestart, &QPushButton::clicked, [this, model]() {
        ui->startRestart->hide();
        ui->winLoseText->hide();
        makeVisibleOnStart();
        int enemies = 5;  // Set your desired initial number of enemies
        int healthpacks = 5;  // Set your desired initial number of health packs
        int xenemies = 2;
        controller->startGame(enemies, healthpacks, xenemies);
    });

    //Connecting button to switch views
    connect(ui->pushButton, &QPushButton::clicked, controller, &GameController::switchView);
    //Connecting keys to move protag
    connect(this, &MainWindow::moveRequested, controller, &GameController::handleMoveRequest);
    //Connects for updating energy and health
    connect(controller, &GameController::energyChanged, this, [this](int newEnergy) {
        ui->progressBar_2->setValue(newEnergy);
    });
    connect(controller, &GameController::healthChanged, this, [this](int newHealth) {
        ui->progressBar->setValue(newHealth);
    });
    //Connect for game over
    connect(controller, &GameController::gameOver, this, &MainWindow::onGameOver);

    //Connect for autoplay
    connect(ui->autoplayButton, &QPushButton::clicked, this, &MainWindow::manageAutoplayStart);

    //Connects for text commands
    // connect(this, &MainWindow::checkInput, this, &MainWindow::manageTextCommands);
    connect(ui->plainTextEdit, &CustomPlainTextEdit::enterPressed, this, &MainWindow::manageTextCommands);
    connect(this, &MainWindow::goToTile, controller, &GameController::handleGoToTile);
    connect(this, &MainWindow::attackNearestEnemy, controller, &GameController::handleAttackNearestEnemy);
    connect(this, &MainWindow::takeNearestHealthpack, controller, &GameController::handleGetNearestHealthpack);
    initializeCommands();

    //Zoom in/out
    connect(ui->zoomInButton, &QPushButton::clicked, this, &MainWindow::on_zoomInButton_clicked);
    connect(ui->zoomOutButton, &QPushButton::clicked, this, &MainWindow::on_zoomOutButton_clicked);

    //Keeping center:
    connect(controller, &GameController::protagonistMovedCenter, this, &MainWindow::updateViewToProtagonist);

    //Slider for pathfinder heuristic weight

    //Install event filter to block events during autoplay
    this->installEventFilter(this);



}

void MainWindow::hideForStart(){
    //Hide all the things
    ui->autoplayButton->hide();
    ui->pushButton->hide();
    ui->zoomInButton->hide();
    ui->zoomOutButton->hide();
    ui->plainTextEdit->hide();
    ui->textBrowser->hide();
    ui->textBrowser_2->hide();
    ui->winLoseText->hide();
    ui->progressBar->hide();
    ui->progressBar_2->hide();
    ui->horizontalSlider->hide();
}

void MainWindow::makeVisibleOnStart(){
    //Make visible all the things when the game has started
    ui->autoplayButton->show();
    ui->pushButton->show();
    ui->zoomInButton->show();
    ui->zoomOutButton->show();
    ui->plainTextEdit->show();
    ui->textBrowser->show();
    ui->textBrowser_2->show();
    // ui->winLoseText->hide();
    ui->progressBar->show();
    ui->progressBar_2->show();
    ui->horizontalSlider->show();
}

void MainWindow::initializeCommands()
{
    // Add commands to the map
    commandMap["go to"] = [this](const QStringList& parts) {
        if (parts.size() == 4) {
            bool xOk, yOk;
            int targetX = parts[2].toInt(&xOk);
            int targetY = parts[3].toInt(&yOk);
            if (xOk && yOk) {
                emit goToTile(targetX, targetY);
            }
        }
    };

    commandMap["attack nearest enemy"] = [this](const QStringList&) {
        emit attackNearestEnemy();
    };

    commandMap["take nearest healthpack"] = [this](const QStringList&) {
        emit takeNearestHealthpack();
    };

    commandMap["help"] = [this](const QStringList&) {
        // List available commands
        QString availableCommands = "Available commands: ";
        for (const auto& cmd : commandMap.keys()) {
            availableCommands += cmd + ", ";
        }
        qDebug() << availableCommands;
    };
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A: emit moveRequested(-1, 0); break;
    case Qt::Key_D: emit moveRequested(1, 0);  break;
    case Qt::Key_W: emit moveRequested(0, -1); break;
    case Qt::Key_S: emit moveRequested(0, 1);  break;
    case Qt::Key_Return: emit checkInput(); break;
    default: QMainWindow::keyPressEvent(event); return;
    }

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGameOver(GameResult result)
{
    //Enable key input
    autoplayActive = false;
    hideForStart();
    ui->winLoseText->show();
    ui->startRestart->show();
    ui->startRestart->setText("Play again");
    if(result == GameResult::Won) {
        ui->winLoseText->setText("Congratulations, You Won!");
    }
    else if(result == GameResult::Lost) {
        ui->winLoseText->setText("Game Over, You Lost!");
    }
    else{
        ui->winLoseText->setText("Strange... The game is incomplete");
    }
}

void MainWindow::manageAutoplayStart()
{
    //Disable key input
    autoplayActive = true;
    connect(this, &MainWindow::startAutoplay, controller, &GameController::startAutoplay);
    emit startAutoplay(); //Take value from slider in the ui
}

void MainWindow::manageTextCommands()
{
    QString command = ui->plainTextEdit->toPlainText();
    ui->plainTextEdit->clear();

    // Find the command in the map and execute it
    for (const auto& cmd : commandMap.keys()) {
        if (command.startsWith(cmd)) {
            QStringList parts = command.split(" ");
            commandMap[cmd](parts);
            return;
        }
    }

    // Command not found
    qDebug() << "Invalid command entered.";
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (autoplayActive) {
        if (event->type() == QEvent::KeyPress) {
            // Optionally, you can check for specific keys here
            return true; // Block the key press event during autoplay
        }
    }
    return QMainWindow::eventFilter(obj, event); // Standard event processing
}

void MainWindow::on_zoomInButton_clicked() {
    ui->graphicsView->scale(1.1, 1.1); // Zoom in by 10%
}

// Slot for zooming out
void MainWindow::on_zoomOutButton_clicked() {
    ui->graphicsView->scale(0.9, 0.9); // Zoom out by 10%
}

void MainWindow::updateViewToProtagonist(int xPos, int yPos)
{
    qDebug() << "HEREEE!";
    const int tileWidth = 25; // Width of a tile in the scene
    const int tileHeight = 25; // Height of a tile in the scene

    // Calculate the center position in the scene
    qreal sceneX = xPos * tileWidth + tileWidth / 2.0;
    qreal sceneY = yPos * tileHeight + tileHeight / 2.0;

    // Center the view on the protagonist's position
    ui->graphicsView->centerOn(sceneX, sceneY);
}

