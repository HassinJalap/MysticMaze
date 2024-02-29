#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "Model/gamemodel.h"
#include "View/baseview.h"
#include "View/graphicsview.h"
#include "View/asciiview.h"
#include "Controller/gamecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();




public slots:
    void onGameOver(GameResult result);
    void manageAutoplayStart();
    void manageTextCommands();
    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();
    void updateViewToProtagonist(int xPos, int yPos);

signals:
    void moveRequested(int xDir, int yDir);
    void startAutoplay();
    void checkInput();
    void goToTile(int xPos, int yPos);
    void attackNearestEnemy();
    void takeNearestHealthpack();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GameController *controller;
    std::shared_ptr<AsciiView> asciiView;
    std::shared_ptr<GraphicsView> graphicsView;
    std::shared_ptr<OverlayView> overlayView;

    bool autoplayActive = false;

    void hideForStart();
    void makeVisibleOnStart();
    void initializeCommands();

    //For text commands
    using CommandFunction = std::function<void(const QStringList&)>;
    QMap<QString, CommandFunction> commandMap;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};
#endif // MAINWINDOW_H
