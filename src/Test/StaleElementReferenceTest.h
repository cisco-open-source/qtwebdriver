#ifndef STALEELEMENTREFERENCETEST_H
#define STALEELEMENTREFERENCETEST_H

#include "CommonQtTestHeaders.h"

class StaleElementReferenceTestWidget:public QWidget
{
    Q_OBJECT

private:
    QLabel *plblCaption;
    QPushButton *ppushBtn;

public:
    StaleElementReferenceTestWidget(QWidget *parent = 0);
    virtual ~StaleElementReferenceTestWidget();
};

#endif // STALEELEMENTREFERENCETEST_H
