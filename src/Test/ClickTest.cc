/* ClickTest.cc
  */

#include "ClickTest.h"

static const QString WINDOW_TITLE = "CLick Test Window";

ClickTestWidget::ClickTestWidget()
{
    this->resize(400, 400);

    this->normalBtn = new QPushButton(this);
    this->normalBtn->setText("Change Title");
    this->normalBtn->move(10, 10);

    this->checkBox = new QCheckBox(this);
    this->checkBox->setText("Change Title");
    this->checkBox->move(40, 40);

    this->scrollArea = new QScrollArea(this);
    this->scrollArea->move(80, 80);
    this->btnOnScroolArea = new QPushButton(this);
    this->btnOnScroolArea->move(80, 80);
    this->scrollArea->setWidget(btnOnScroolArea);

    connect(this->normalBtn, SIGNAL(clicked()), this, SLOT(OnNormalBtnClick()));
    connect(this->checkBox, SIGNAL(clicked()), this, SLOT(OnCheckBoxClick()));
    connect(this->btnOnScroolArea, SIGNAL(clicked()), this, SLOT(OnBtnOnScrollClick()));

    this->normalBtn->setObjectName("pushBtn");
    this->checkBox->setObjectName("checkBox");
    this->btnOnScroolArea->setObjectName("btnOnScroll");

    this->setWindowTitle("ClickTest");
}

ClickTestWidget::~ClickTestWidget()
{}

void ClickTestWidget::OnNormalBtnClick()
{
    this->setWindowTitle(WINDOW_TITLE);
}

void ClickTestWidget::OnCheckBoxClick()
{
    if (checkBox->isChecked())
        this->setWindowTitle(WINDOW_TITLE);
}

void ClickTestWidget::OnBtnOnScrollClick()
{
    this->setWindowTitle(WINDOW_TITLE);
}
