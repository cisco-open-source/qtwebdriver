/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "XPathElementFindingTest.h"

XPathElementFindingTestWidget::XPathElementFindingTestWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle(QString::fromStdString("Application Window"));

    QLabel *plblCaption = new QLabel("This Is Caption");
    plblCaption->setObjectName("captionLabel");

    pradioBtn1 = new QRadioButton("First",this);
    pradioBtn1->setChecked(true); //selected by default
    pradioBtn1->setObjectName("defaultSelected");

    pradioBtn2 = new QRadioButton("Second",this);
    pradioBtn2->setObjectName("noSelected");

    ppushBtn = new QPushButton("Click here");
    ppushBtn->setObjectName("buttonInScrollArea");

    pscrollArea = new QScrollArea(this);
    pscrollArea->setObjectName("scrollArea");
    pscrollArea->setWidget(ppushBtn);

    pinputElemWithText = new QLineEdit("Example text", this);
    pinputElemWithText->setObjectName("inputElement");

    pinputElemEmpty = new QLineEdit(this);
    pinputElemEmpty->setObjectName("inputEmpty");

    pinputElemReadOnly = new QLineEdit("Read Only", this);
    pinputElemReadOnly->setEnabled(false);
    pinputElemReadOnly->setObjectName("inputDisabled");

    ptxtAreaWorking = new QTextEdit(this);
    ptxtAreaWorking->setPlainText("Example text");
    ptxtAreaWorking->setObjectName("workingArea");

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    pvbl->addWidget(plblCaption);

    QHBoxLayout *phbl = new QHBoxLayout;
    phbl->addWidget(pradioBtn1);
    phbl->addWidget(pradioBtn2);
    pvbl->addLayout(phbl);

    pvbl->addWidget(pinputElemWithText);
    pvbl->addWidget(pinputElemEmpty);
    pvbl->addWidget(pinputElemReadOnly);
    pvbl->addWidget(ptxtAreaWorking);
    pvbl->addWidget(pscrollArea);

    this->setLayout(pvbl);
}

XPathElementFindingTestWidget::~XPathElementFindingTestWidget()
{
}
