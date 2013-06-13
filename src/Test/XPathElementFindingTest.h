#ifndef XPATHELEMENTFINDINGTEST_H
#define XPATHELEMENTFINDINGTEST_H

#include "CommonQtTestHeaders.h"

class XPathElementFindingTestWidget : public QWidget
{
    Q_OBJECT

private:
    QPushButton *ppushBtn;
    QTextEdit *ptxtAreaWorking;
    QLineEdit *pinputElemWithText;
    QLineEdit *pinputElemEmpty;
    QLineEdit *pinputElemReadOnly;
    QScrollArea *pscrollArea;
    QRadioButton *pradioBtn1;
    QRadioButton *pradioBtn2;

public:
    explicit XPathElementFindingTestWidget(QWidget *parent = 0);
    virtual ~XPathElementFindingTestWidget();
        
};

#endif // XPATHELEMENTFINDINGTEST_H
