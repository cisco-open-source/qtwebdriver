#ifndef BASICKEYBOARDINTERFACETEST_H
#define BASICKEYBOARDINTERFACETEST_H


#include "CommonQtTestHeaders.h"

class LEdit;

class BasicKeyboardInterfaceTestWidget: public QWidget
{
    Q_OBJECT

public:
    explicit BasicKeyboardInterfaceTestWidget();
    virtual ~BasicKeyboardInterfaceTestWidget();

private slots:
    void setResultModifier(const QString &str);
    void setResultDown();
    void setResultUp();

private:
    QLabel *plblResultModifier;
    QLabel *plblResultIsKeyDown;
    QLabel *plblResultIsKeyUp;
    LEdit *plnEdit;

};

class LEdit: public QLineEdit {
    Q_OBJECT

private:
    QString currentText;

public:
    LEdit(QWidget * parent = 0);
    LEdit(const QString & contents, QWidget * parent = 0);
    virtual ~LEdit();

signals:
    void keyDown();
    void keyUp();
    void keyModifier(QString str);

public:
    virtual void keyPressEvent(QKeyEvent *pe);
    virtual void keyReleaseEvent(QKeyEvent *pe);
};


#endif // BASICKEYBOARDINTERFACETEST_H
