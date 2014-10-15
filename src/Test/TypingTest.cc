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

