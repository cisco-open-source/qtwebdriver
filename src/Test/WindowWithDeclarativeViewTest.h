#ifndef WINDOWWITHDECLARATIVEVIEWTEST_H
#define WINDOWWITHDECLARATIVEVIEWTEST_H

#include "CommonQtTestHeaders.h"

#include <QtDeclarative/QDeclarativeView>

class WindowWithDeclarativeViewTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowWithDeclarativeViewTestWidget(QWidget *parent = 0);
    ~WindowWithDeclarativeViewTestWidget();
    
public slots:
    void loadQML();
    void displayStatus();

private:
    QPushButton* pButton;
    QLineEdit* pLineEdit;
    QLabel *pLabel;
    QDeclarativeView *pView;


};

#endif // WINDOWWITHDECLARATIVEVIEWTEST_H
