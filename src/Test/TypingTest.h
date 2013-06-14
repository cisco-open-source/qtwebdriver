#ifndef TYPINGTEST_H
#define TYPINGTEST_H

#include "CommonQtTestHeaders.h"

class TypingTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TypingTestWidget();
    virtual ~TypingTestWidget();
    
public slots:
    void setResult(QString str);

private:
    QLabel *plblNoChange;
    QLabel *plblResult;
    QLineEdit *ptxtEn;
    QPlainTextEdit *ptxtAreaWorking;
    
};

#endif // TYPINGTEST_H
