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
