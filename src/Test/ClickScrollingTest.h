#ifndef CLICKSCROLLINGTEST_H
#define CLICKSCROLLINGTEST_H

#include <QtGui/QMainWindow>

namespace Ui {
class ClickScrollingTest;
}

class ClickScrollingTest : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ClickScrollingTest(QWidget *parent = 0);
    ~ClickScrollingTest();
    
private:
    Ui::ClickScrollingTest *ui;
private slots:
    void ClickOnVisibleButton();
    void ClickOnButtonHiddenByOverflow();
    void ClickOnPartiallyVisibleRadioButton();
};

#endif // CLICKSCROLLINGTEST_H
