#ifndef TOUCHTEST_H
#define TOUCHTEST_H

#include "CommonQtTestHeaders.h"

class TouchTestWidget : public QWidget
{
public:
    explicit TouchTestWidget(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    QFrame* touchPanel;
    QLabel* resultLabel;
    QLabel* pressLabel;
    QLabel* releaseLabel;
    QLabel* moveLabel;
};

#endif // TOUCHTEST_H
