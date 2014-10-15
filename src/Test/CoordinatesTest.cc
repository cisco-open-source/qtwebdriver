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

/*
  CoordinatesTest.cc
  */
#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QGridLayout>
#else
#include <QtGui/QGridLayout>
#endif

#include "CoordinatesTest.h"

static const QString SCROLL_AREA_NAME="scrollArea";
static const QString ZERO_SIZE_LABEL_NAME="zeroSizeLabel";
static const QString TRANSPARENT_LABEL_NAME="transparentLabel";
static const QString HIDDEN_LABEL_NAME="hiddenLabel";
static const QString UNVISIBLE_LABEL_NAME="unvisibleLabel";

CoordinatesTestWidget::CoordinatesTestWidget()
{
    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    QWidget *contentWidget = new QWidget();
    contentWidget->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    scrollArea = new QScrollArea();
    scrollArea->setObjectName(SCROLL_AREA_NAME);
    scrollArea->setGeometry(QRect(10, 10, 400, 250));
    scrollArea->setWidget(contentWidget);

    zeroSizeLabel = new QLabel;
    zeroSizeLabel->setObjectName(ZERO_SIZE_LABEL_NAME);
    zeroSizeLabel->setFixedSize(0, 0);
    zeroSizeLabel->setGeometry(QRect(8, 8, 0, 0));
    scrollArea->widget()->layout()->addWidget(zeroSizeLabel);

    transparentLabel = new QLabel();
    transparentLabel->setObjectName(TRANSPARENT_LABEL_NAME);
    transparentLabel->setWindowOpacity(0);
    scrollArea->widget()->layout()->addWidget(transparentLabel);

    // Add label just for offset
    QLabel *offsetLabel = new QLabel();
    offsetLabel->setFixedHeight(200);
    scrollArea->widget()->layout()->addWidget(offsetLabel);

    hiddenLabel = new QLabel();
    hiddenLabel->setObjectName(HIDDEN_LABEL_NAME);
    hiddenLabel->setGeometry(QRect(600, 600, 20, 20));
    scrollArea->widget()->layout()->addWidget(hiddenLabel);

    unvisibleLabel = new QLabel();
    unvisibleLabel->setObjectName(UNVISIBLE_LABEL_NAME);
    unvisibleLabel->setVisible(false);

    layout->addWidget(scrollArea, 0, 1);
    layout->addWidget(unvisibleLabel, 1, 0);
}

CoordinatesTestWidget::~CoordinatesTestWidget()
{
}
