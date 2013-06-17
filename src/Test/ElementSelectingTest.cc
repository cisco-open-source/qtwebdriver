#include "ElementSelectingTest.h"
#include "ui_ElementSelectingTest.h"

ElementSelectingTest::ElementSelectingTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ElementSelectingTest)
{
    ui->setupUi(this);
}

ElementSelectingTest::~ElementSelectingTest()
{
    delete ui;
}
