QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Controller/gamecontroller.cpp \
    Model/gamemodel.cpp \
    Model/growingbehavior.cpp \
    Model/movingbehavior.cpp \
    Model/teleportbehavior.cpp \
    Model/xenemy.cpp \
    View/asciiview.cpp \
    View/graphicsview.cpp \
    View/overlayview.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Controller/gamecontroller.h \
    Model/behaviorstrategy.h \
    Model/gamemodel.h \
    Model/growingbehavior.h \
    Model/movingbehavior.h \
    Model/teleportbehavior.h \
    Model/xenemy.h \
    View/baseview.h \
    View/asciiview.h \
    View/graphicsview.h \
    View/overlayview.h \
    customplaintextedit.h \
    mainwindow.h

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix:!macx: LIBS += -L$$PWD/../../../../../Downloads/worldlib/ -lworld

INCLUDEPATH += $$PWD/../../../../../Downloads/worldlib
DEPENDPATH += $$PWD/../../../../../Downloads/worldlib

RESOURCES += \
    resources.qrc
