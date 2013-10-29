#ifndef WIDGETANDWEBVIEWTEST_H
#define WIDGETANDWEBVIEWTEST_H

#include "CommonQtTestHeaders.h"

class WidgetAndWebViewTestWindows : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetAndWebViewTestWindows(QWidget *parent = 0);
    ~WidgetAndWebViewTestWindows();
    
public slots:
    void openWebView();

private:
    QPushButton* pButton;
    QLineEdit* pLineEdit;
    
};

#endif // WIDGETANDWEBVIEWTEST_H
