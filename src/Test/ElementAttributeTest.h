#ifndef ELEMENTATTRIBUTETEST_H
#define ELEMENTATTRIBUTETEST_H

#include "CommonQtTestHeaders.h"

class ElementAttributeTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ElementAttributeTestWidget();
    virtual ~ElementAttributeTestWidget();

private:
    QLabel *plblDis;
    QLabel *plblEn;
    QPushButton *pbtnEnabled;
    QPushButton *pbtnDisabled;
    QLineEdit *ptxtDis1;
    QLineEdit *ptxtDis2;
    QLineEdit *ptxtEn;
    QPlainTextEdit *ptxtAreaWithDefaultText;
    QPlainTextEdit *ptxtAreaWorking;
    QCheckBox *pchboxWithText;
    QCheckBox *pchboxWithoutText;
    QRadioButton *pradioBtnBlue;
    QRadioButton *pradioBtnYellow;
    
};

#endif // ELEMENTATTRIBUTETEST_H
