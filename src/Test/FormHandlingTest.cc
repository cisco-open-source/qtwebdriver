#include "FormHandlingTest.h"
#include "img.xpm"

FormHandlingTestWidget::FormHandlingTestWidget(QWidget *parent) :
    QWidget(parent)
{  
    this->setWindowTitle(QString::fromStdString("We Leave From Here"));

    QLabel *plbl = new QLabel("Label not clickable",this);
    plbl->setObjectName("notClickableLabel");

    pTxtBtn = new QPushButton("Change Title",this);
    pTxtBtn->setObjectName("changeTitleButton");
    connect(pTxtBtn, SIGNAL(clicked()), this, SLOT(setNewWindowTitle()));

    pImageBtn = new QPushButton(this);
    pImageBtn->setObjectName("imageButton");
    pImageBtn->setIcon(QPixmap(img_xpm));
    connect(pImageBtn, SIGNAL(clicked()), this, SLOT(setNewWindowTitle()));

    pinputElemWithText = new QLineEdit("Example text", this);
    pinputElemWithText->setObjectName("inputElement");

    pinputElemEmpty = new QLineEdit(this);
    pinputElemEmpty->setObjectName("emptyInput");

    ptxtAreaWorking = new QPlainTextEdit(this);
    ptxtAreaWorking->setPlainText("Example text");
    ptxtAreaWorking->setObjectName("workingArea");

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    QHBoxLayout *phbl = new QHBoxLayout;
    phbl->addWidget(plbl);
    phbl->addWidget(pTxtBtn);
    phbl->addWidget(pImageBtn);
    pvbl->addLayout(phbl);

    pvbl->addWidget(pinputElemWithText);
    pvbl->addWidget(pinputElemEmpty);

    pvbl->addWidget(ptxtAreaWorking);

    this->setLayout(pvbl);
}

FormHandlingTestWidget::~FormHandlingTestWidget()
{
}

void FormHandlingTestWidget::setNewWindowTitle()
{
    this->setWindowTitle(QString::fromStdString("We Arrive Here"));
}
