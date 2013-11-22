#ifndef MENU_TEST_H_
#define MENU_TEST_H_

#include "CommonQtTestHeaders.h"

class MenuTestWidget : public QMainWindow
{
    Q_OBJECT

public:
    MenuTestWidget(QWidget *parent = 0);

protected slots:

    void updateStatus(QString text);
private:
    QLabel *plblStatus;
};

#endif // MENU_TEST_H_
