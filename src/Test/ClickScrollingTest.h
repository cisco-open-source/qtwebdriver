#ifndef CLICKSCROLLINGTEST_H
#define CLICKSCROLLINGTEST_H

#include "CommonQtTestHeaders.h"

namespace Ui {
class ClickScrollingTestWidget;
}

class ClickScrollingTestWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ClickScrollingTestWidget(QWidget *parent = 0);
    ~ClickScrollingTestWidget();
    
private:
    Ui::ClickScrollingTestWidget *ui;
private slots:
    void ClickOnVisibleButton();
    void ClickOnButtonHiddenByOverflow();
    void ClickOnPartiallyVisibleRadioButton();
};

#endif // CLICKSCROLLINGTEST_H
