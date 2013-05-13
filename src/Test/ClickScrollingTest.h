#ifndef CLICKSCROLLINGTEST_H
#define CLICKSCROLLINGTEST_H



#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

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
