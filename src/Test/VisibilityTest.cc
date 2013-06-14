#include "VisibilityTest.h"

VisibilityTestWidget::VisibilityTestWidget(QWidget *parent) :
    QWidget(parent)
{
    plblDis = new QLabel("No Visible Label", this);
    plblDis->setObjectName("notDisplayedLabel");
    plblDis->setVisible(false);

    plblEn = new QLabel("Visible Label", this);
    plblEn->setObjectName("displayedLabel");
    plblEn->setVisible(true);

    plblZero = new QLabel(this);
    plblZero->setObjectName("zeroLabel");
    plblZero->setVisible(true);
    plblZero->setMaximumSize(0,0);

    pbtnCanHide = new QPushButton("Glick to hide", this);
    pbtnCanHide->setObjectName("buttonCanHide");
    connect(pbtnCanHide, SIGNAL(clicked()), this, SLOT(hideButton()));

    pbtnHidden = new QPushButton("Hidden Button", this);
    pbtnHidden->setObjectName("buttonHidden");
    pbtnHidden->setVisible(false);

    ptxtHidden = new QLineEdit(this);
    ptxtHidden->setVisible(false);
    ptxtHidden->setObjectName("inputHidden");

    ppushBtn = new QPushButton("Click here");
    ppushBtn->setObjectName("buttonInScrollArea");

    pscrollArea = new QScrollArea(this);
    pscrollArea->setObjectName("hiddenScrollArea");
    pscrollArea->setWidget(ppushBtn);
    pscrollArea->setVisible(false);

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    QHBoxLayout *phblToLabel = new QHBoxLayout;
    phblToLabel->addWidget(plblDis);
    phblToLabel->addWidget(plblEn);
    phblToLabel->addWidget(plblZero);
    pvbl->addLayout(phblToLabel);

    QHBoxLayout *phblToPushButton = new QHBoxLayout;
    phblToPushButton->addWidget(pbtnCanHide);
    phblToPushButton->addWidget(pbtnHidden);
    pvbl->addLayout(phblToPushButton);

    pvbl->addWidget(ptxtHidden);
    pvbl->addWidget(pscrollArea);
    this->setLayout(pvbl);
}

VisibilityTestWidget::~VisibilityTestWidget()
{
}

void VisibilityTestWidget::hideButton()
{
    pbtnCanHide->setVisible(false);
}
