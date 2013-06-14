#ifndef FORMHANDLINGTEST_H
#define FORMHANDLINGTEST_H

#include "CommonQtTestHeaders.h"

class FormHandlingTestWidget : public QWidget
{
    Q_OBJECT

private:
    QPushButton *pTxtBtn;
    QPushButton *pImageBtn;
    QPlainTextEdit *ptxtAreaWorking;
    QLineEdit *pinputElemWithText;
    QLineEdit *pinputElemEmpty;

public:
    explicit FormHandlingTestWidget(QWidget *parent = 0);
    virtual ~FormHandlingTestWidget();
    
signals:
    
private slots:
    void setNewWindowTitle();
    
};

#endif // FORMHANDLINGTEST_H
