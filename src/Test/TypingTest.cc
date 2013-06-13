#include "TypingTest.h"

TypingTestWidget::TypingTestWidget() :
    QWidget()
{
    plblNoChange = new QLabel("You changed: ",this);
    plblResult = new QLabel("default", this);
    plblResult->setObjectName("result");

    ptxtEn= new QLineEdit(this);
    ptxtEn->setObjectName("enabeledTextElement");
    connect(ptxtEn, SIGNAL(textChanged(QString)), this, SLOT(setResult(QString)));

    ptxtAreaWorking = new QPlainTextEdit(this);
    ptxtAreaWorking->setEnabled(true);
    ptxtAreaWorking->setObjectName("workingArea");

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    QHBoxLayout *phblToLabel = new QHBoxLayout;
    phblToLabel->addWidget(plblNoChange);
    phblToLabel->addWidget(plblResult);
    pvbl->addLayout(phblToLabel);

    pvbl->addWidget(ptxtEn);

    pvbl->addWidget(ptxtAreaWorking);

    this->setLayout(pvbl);
}

TypingTestWidget::~TypingTestWidget()
{
}

void TypingTestWidget::setResult(QString str)
{
    plblResult->setText(str);
}

