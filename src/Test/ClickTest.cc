/* ClickTest.cc
  */

#include "ClickTest.h"

static const QString WINDOW_TITLE = "CLick Test Window";

ClickTestWidget::ClickTestWidget()
{
    this->normalBtn = new QPushButton(this);
    this->checkBox = new QCheckBox(this);
    this->scrollArea = new QScrollArea(this);
    this->btnOnScroolArea = new QPushButton(this);

    connect(this->normalBtn, SIGNAL(clicked()), this, SLOT(OnNormalBtnClick()));
    connect(this->checkBox, SIGNAL(clicked()), this, SLOT(OnCheckBoxClick()));
    connect(this->btnOnScroolArea, SIGNAL(clicked()), this, SLOT(OnBtnOnScrollClick()));

    this->normalBtn->setObjectName("pushBtn");
    this->checkBox->setObjectName("checkBox");
    this->btnOnScroolArea->setObjectName("btnOnScroll");
    this->scrollArea->setViewport(btnOnScroolArea);

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
