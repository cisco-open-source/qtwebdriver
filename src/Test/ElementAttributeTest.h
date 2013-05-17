#ifndef ELEMENTATTRIBUTETEST_H
#define ELEMENTATTRIBUTETEST_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>


class ElementAttributeTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ElementAttributeTestWidget();
    ~ElementAttributeTestWidget();

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
