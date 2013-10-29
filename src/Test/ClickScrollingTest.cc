#include "ClickScrollingTest.h"
#include "ui_ClickScrollingTest.h"
#include <QtGui/QRegion>

ClickScrollingTestWidget::ClickScrollingTestWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClickScrollingTestWidget)
{
    ui->setupUi(this);

    connect(this->ui->visibleButton, SIGNAL(clicked()), this, SLOT(ClickOnVisibleButton()));
    connect(this->ui->buttonHiddenByOverflow, SIGNAL(clicked()), this, SLOT(ClickOnButtonHiddenByOverflow()));
    connect(this->ui->partiallyVisibleRadioButton, SIGNAL(clicked()), this, SLOT(ClickOnPartiallyVisibleRadioButton()));
}

ClickScrollingTestWidget::~ClickScrollingTestWidget()
{
    delete ui;
}

void ClickScrollingTestWidget::ClickOnVisibleButton()
{

}

void ClickScrollingTestWidget::ClickOnButtonHiddenByOverflow()
{
    this->ui->buttonHiddenByOverflow->setText("clicked");
}

void ClickScrollingTestWidget::ClickOnPartiallyVisibleRadioButton()
{
    this->ui->partiallyVisibleRadioButton->setText("clicked");
}
