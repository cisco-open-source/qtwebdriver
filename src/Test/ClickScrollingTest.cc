#include "ClickScrollingTest.h"
#include "ui_ClickScrollingTest.h"
#include <QtGui/QRegion>

ClickScrollingTest::ClickScrollingTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClickScrollingTest)
{
    ui->setupUi(this);

    connect(this->ui->visibleButton, SIGNAL(clicked()), this, SLOT(ClickOnVisibleButton()));
    connect(this->ui->buttonHiddenByOverflow, SIGNAL(clicked()), this, SLOT(ClickOnButtonHiddenByOverflow()));
    connect(this->ui->partiallyVisibleRadioButton, SIGNAL(clicked()), this, SLOT(ClickOnPartiallyVisibleRadioButton()));
}

ClickScrollingTest::~ClickScrollingTest()
{
    delete ui;
}

void ClickScrollingTest::ClickOnVisibleButton()
{

}

void ClickScrollingTest::ClickOnButtonHiddenByOverflow()
{
    this->ui->buttonHiddenByOverflow->setText("clicked");
}

void ClickScrollingTest::ClickOnPartiallyVisibleRadioButton()
{
    this->ui->partiallyVisibleRadioButton->setText("clicked");
}
