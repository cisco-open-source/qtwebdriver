/* ClickTest.h
  */

#ifndef CLICKTEST_H
#define CLICKTEST_H

#include "CommonQtTestHeaders.h"

class ClickTestWidget : public QWidget
{
    Q_OBJECT
public:
    ClickTestWidget();
    virtual ~ClickTestWidget();

private slots:
    void OnNormalBtnClick();
    void OnCheckBoxClick();
    void OnBtnOnScrollClick();

private:
    QPushButton *normalBtn;
    QCheckBox *checkBox;
    QScrollArea *scrollArea;
    QPushButton *btnOnScroolArea;
};

#endif // CLICKTEST_H
