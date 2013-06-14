#include "StaleElementReferenceTest.h"

StaleElementReferenceTestWidget::StaleElementReferenceTestWidget(QWidget *parent):
        QWidget(parent)
{
    plblCaption = new QLabel("This Is Caption", this);
    plblCaption->setObjectName("captionLabel");

    ppushBtn = new QPushButton("Click me", this);
    ppushBtn->setObjectName("clickButton");

    QVBoxLayout *pvbl = new QVBoxLayout(this);
    pvbl->addWidget(plblCaption);
    pvbl->addWidget(ppushBtn);
    this->setLayout(pvbl);

}

StaleElementReferenceTestWidget::~StaleElementReferenceTestWidget()
{
}
