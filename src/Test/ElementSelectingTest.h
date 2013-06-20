#ifndef ELEMENTSELECTINGTEST_H
#define ELEMENTSELECTINGTEST_H

#include "CommonQtTestHeaders.h"

namespace Ui {
class ElementSelectingTestWidget;
}

class ElementSelectingTestWidget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ElementSelectingTestWidget(QWidget *parent = 0);
    ~ElementSelectingTestWidget();
    
private:
    Ui::ElementSelectingTestWidget *ui;
};

#endif // ELEMENTSELECTINGTEST_H
