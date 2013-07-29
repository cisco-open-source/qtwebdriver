#ifndef WINDOWWITHEMBEDDEDVIEWTEST_H
#define WINDOWWITHEMBEDDEDVIEWTEST_H

#include "CommonQtTestHeaders.h"

class QWebView;

class WindowWithEmbeddedViewTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowWithEmbeddedViewTestWidget(QWidget *parent = 0);
    ~WindowWithEmbeddedViewTestWidget();
    
public slots:
    void loadHTML();
    void setHTMLTitle();

private:
    QPushButton* pButton;
    QLineEdit* pLineEdit;
    QLabel *pLabel;
    QWebView *pView;
    
};

#endif // WINDOWWITHEMBEDDEDVIEWTEST_H
