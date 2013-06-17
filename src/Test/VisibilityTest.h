#ifndef VISIBILITYTEST_H
#define VISIBILITYTEST_H

#include "CommonQtTestHeaders.h"

class VisibilityTestWidget : public QWidget
{
    Q_OBJECT

private:
    QLabel *plblDis;
    QLabel *plblEn;
    QLabel *plblZero;
    QPushButton *pbtnCanHide;
    QPushButton *pbtnHidden;
    QLineEdit *ptxtHidden;
    QScrollArea *pscrollArea;
    QPushButton *ppushBtn;

public:
    explicit VisibilityTestWidget(QWidget *parent = 0);
    virtual ~VisibilityTestWidget();
    
public slots:
    void hideButton();
    
};

#endif // VISIBILITYTEST_H
