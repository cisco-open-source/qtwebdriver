#ifndef WINDOWWITHDECLARATIVEVIEWTEST_H
#define WINDOWWITHDECLARATIVEVIEWTEST_H

#include "CommonQtTestHeaders.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtQuick/QQuickView>
#else
#include <QtDeclarative/QDeclarativeView>
#endif


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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QQuickView *pView;
#else
    QDeclarativeView *pView;
#endif

};

#endif // WINDOWWITHDECLARATIVEVIEWTEST_H
