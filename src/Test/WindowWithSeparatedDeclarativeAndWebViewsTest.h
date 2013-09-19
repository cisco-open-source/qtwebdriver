#ifndef WINDOWWITHSEPARATEDDECLARATIVEANDWEBVIEWSTEST_H
#define WINDOWWITHSEPARATEDDECLARATIVEANDWEBVIEWSTEST_H

#include "CommonQtTestHeaders.h"

class WindowWithSeparatedDeclarativeAndWebViewsTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowWithSeparatedDeclarativeAndWebViewsTestWidget(QWidget *parent = 0);
    ~WindowWithSeparatedDeclarativeAndWebViewsTestWidget();
    
public slots:
    void openWebView();
    void openQmlView();

private:
    QPushButton* pButtonWeb;
    QLineEdit* pLineEditForWeb;
    QPushButton* pButtonQml;
    QLineEdit* pLineEditForQml;
    
};

#endif // WINDOWWITHSEPARATEDDECLARATIVEANDWEBVIEWSTEST_H
