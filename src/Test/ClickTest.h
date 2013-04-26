/* ClickTest.h
  */

#ifndef CLICKTEST_H
#define CLICKTEST_H

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QScrollArea>
#else
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QScrollArea>
#endif

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
