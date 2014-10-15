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

/* FindingTest.cc
  */

#include "FindingTest.h"

#include <QtCore/QDebug>

static QString TEXT_EDIT_NAME="textEdit";
static QString SCROLL_AREA_NAME="scrollArea";
static QString HIDDEN_LABEL_NAME="hiddenLabel";

FindingTestWidget::FindingTestWidget()
{
    textEdit = new QTextEdit();
    scrollArea = new QScrollArea();
    hiddenLabel = new QLabel();

    textEdit->setObjectName(TEXT_EDIT_NAME);
    scrollArea->setObjectName(SCROLL_AREA_NAME);
    hiddenLabel->setObjectName(HIDDEN_LABEL_NAME);

    QRect labelGeometry(scrollArea->geometry());
    labelGeometry.setTopLeft(QPoint(30, 30));
    hiddenLabel->setGeometry(labelGeometry);
    scrollArea->setWidget(hiddenLabel);

    QLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    this->setLayout(layout);
    layout->addWidget(textEdit);
    layout->addWidget(scrollArea);
}

FindingTestWidget::~FindingTestWidget()
{
}
