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

#include "ElementAttributeTest.h"

ElementAttributeTestWidget::ElementAttributeTestWidget() :
    QWidget()
{
    plblDis = new QLabel(this);
    plblDis->setObjectName("notDisplayedLabel");
    plblDis->setText("No Visible Label");
    plblDis->setVisible(false);

    plblEn = new QLabel(this);
    plblEn->setText("Visible Label");
    plblEn->setObjectName("displayedLabel");
    plblEn->setVisible(true);

    pbtnEnabled = new QPushButton(this);
    pbtnEnabled->setEnabled(true);
    pbtnEnabled->setObjectName("workingButton");
    pbtnEnabled->setText("Working Button");
    plblEn->setBuddy(pbtnEnabled);

    pbtnDisabled = new QPushButton("No Working Button", this);
    pbtnDisabled->setEnabled(false);
    pbtnDisabled->setVisible(false);
    pbtnDisabled->setObjectName("notWorkingButton");
    plblDis->setBuddy(pbtnDisabled);

    ptxtDis1 = new QLineEdit(this);
    ptxtDis1->setEnabled(false);
    ptxtDis1->setObjectName("disabledTextElement1");

    ptxtDis2 = new QLineEdit(this);
    ptxtDis2->setEnabled(false);
    ptxtDis2->setObjectName("disabledTextElement2");

    ptxtEn= new QLineEdit(this);
    ptxtEn->setObjectName("enabeledTextElement");

    ptxtAreaWithDefaultText = new QPlainTextEdit(this);
    ptxtAreaWithDefaultText->setEnabled(false);
    ptxtAreaWithDefaultText->setObjectName("areaWithDefaultText");
    ptxtAreaWithDefaultText->setPlainText("Example text");
    ptxtAreaWithDefaultText->setReadOnly(true);

    ptxtAreaWorking = new QPlainTextEdit(this);
    ptxtAreaWorking->setEnabled(true);
    ptxtAreaWorking->setObjectName("workingArea");

    pradioBtnBlue = new QRadioButton("Blue",this);
    pradioBtnBlue->setChecked(true); //selected by default
    pradioBtnBlue->setCheckable(true);
    pradioBtnBlue->setObjectName("defaultSelected");

    QPalette* palette1 = new QPalette();
    palette1->setColor(QPalette::Foreground,Qt::darkBlue);
    pradioBtnBlue->setPalette(*palette1);
    pradioBtnBlue->setStyleSheet("background-color: blue");

    pradioBtnYellow = new QRadioButton(/*"Yellow",*/this);
    pradioBtnYellow->setObjectName("noSelected");
    pradioBtnYellow->setCheckable(true);
    pradioBtnYellow->setStyleSheet("background-color: yellow");

    pchboxWithText = new QCheckBox("CheckBox",this);
    pchboxWithText->setObjectName("checkBoxWithText");
    pchboxWithText->setCheckable(true);

    pchboxWithoutText = new QCheckBox(this);
    pchboxWithoutText->setObjectName("checkBoxWithoutText");
    pchboxWithoutText->setCheckable(true);

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    QHBoxLayout *phblToLabelAndPushButton = new QHBoxLayout;
    phblToLabelAndPushButton->addWidget(plblDis);
    phblToLabelAndPushButton->addWidget(pbtnDisabled);
    phblToLabelAndPushButton->addWidget(plblEn);
    phblToLabelAndPushButton->addWidget(pbtnEnabled);
    pvbl->addLayout(phblToLabelAndPushButton);

    pvbl->addWidget(ptxtEn);

    QHBoxLayout *phblToLineEdit = new QHBoxLayout;
    phblToLineEdit->addWidget(ptxtDis1);
    phblToLineEdit->addWidget(ptxtDis2);
    pvbl->addLayout(phblToLineEdit);

    pvbl->addWidget(ptxtAreaWithDefaultText);
    pvbl->addWidget(ptxtAreaWorking);

    QHBoxLayout *phblCheckBoxAndRadioButton = new QHBoxLayout;
    phblCheckBoxAndRadioButton->addWidget(pchboxWithText);
    phblCheckBoxAndRadioButton->addWidget(pchboxWithoutText);
    phblCheckBoxAndRadioButton->addWidget(pradioBtnBlue);
    phblCheckBoxAndRadioButton->addWidget(pradioBtnYellow);
    pvbl->addLayout(phblCheckBoxAndRadioButton);

    this->setLayout(pvbl);

}

ElementAttributeTestWidget::~ElementAttributeTestWidget()
{
}


