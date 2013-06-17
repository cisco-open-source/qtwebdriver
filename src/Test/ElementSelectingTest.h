#ifndef ELEMENTSELECTINGTEST_H
#define ELEMENTSELECTINGTEST_H

#include "CommonQtTestHeaders.h"

namespace Ui {
class ElementSelectingTest;
}

class ElementSelectingTest : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ElementSelectingTest(QWidget *parent = 0);
    ~ElementSelectingTest();
    
private:
    Ui::ElementSelectingTest *ui;
};

#endif // ELEMENTSELECTINGTEST_H
