#include "ElementSelectingTest.h"
#include "ui_ElementSelectingTest.h"

ElementSelectingTestWidget::ElementSelectingTestWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ElementSelectingTestWidget)
{
    ui->setupUi(this);
}

ElementSelectingTestWidget::~ElementSelectingTestWidget()
{
    delete ui;
}
