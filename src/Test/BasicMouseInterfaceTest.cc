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

#include "BasicMouseInterfaceTest.h"
#include "DragableWidget.h"
#include "RestyledLabel.h"
#include "CommonQtTestHeaders.h"

BasicMouseInterfaceTestWidget::BasicMouseInterfaceTestWidget(QWidget *parent) :
    QWidget(parent)
{
    DragableWidget *firstWidget = new DragableWidget(this);
    firstWidget->setObjectName("firstWidget");
    QObjectList childList = firstWidget->children();
    foreach(QObject* child, childList){
        if(dynamic_cast<QLabel*>(child) != NULL){
            QLabel *tmpLabel = dynamic_cast<QLabel*>(child);
            tmpLabel->setObjectName(tmpLabel->parent()->objectName()
                                 .append("_").append(tmpLabel->text()));
            tmpLabel->setText(tmpLabel->objectName());
        }
    }

    DragableWidget *secondWidget = new DragableWidget(this);
    secondWidget->setObjectName("secondWidget");
    childList = secondWidget->children();
    foreach(QObject* child, childList){
        if(dynamic_cast<QLabel*>(child) != NULL){
            QLabel *tmpLabel = dynamic_cast<QLabel*>(child);
            tmpLabel->setObjectName(tmpLabel->parent()->objectName()
                                 .append("_").append(tmpLabel->text()));
            tmpLabel->setText(tmpLabel->objectName());
        }
    }

    QLabel *eventInfo = new QLabel("Nothing happened.", this);
    eventInfo->setObjectName("dragging_reports");
    firstWidget->setDraggingReportsDestination(eventInfo);
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    this->setLayout(vLayout);
    hLayout->addWidget(firstWidget);
    hLayout->addWidget(secondWidget);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(eventInfo);
}
