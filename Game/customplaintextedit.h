#ifndef CUSTOMPLAINEXTEDIT_H
#define CUSTOMPLAINEXTEDIT_H

#endif // CUSTOMPLAINEXTEDIT_H

// CustomPlainTextEdit.h
#include <QPlainTextEdit>
#include <QKeyEvent>

class CustomPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    using QPlainTextEdit::QPlainTextEdit;  // Inherit constructor

signals:
    void enterPressed();

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            emit enterPressed();
        } else {
            QPlainTextEdit::keyPressEvent(event); // Handle other key events normally
        }
    }
};
