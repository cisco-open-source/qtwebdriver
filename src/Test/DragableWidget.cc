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

#include <cstddef>

#include "DragableWidget.h"

DragableWidget::DragableWidget(QWidget *parent)
    : QWidget(parent)
{
    this->isReportDraggingOn = false;

    this->setObjectName("DragWidget");
    QVBoxLayout * verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    verticalLayout->setObjectName("verticalLayout");

    RestyledLabel *label_1 = new RestyledLabel("label_1", this);
    label_1->show();
    label_1->setAttribute(Qt::WA_DeleteOnClose);

    RestyledLabel *label_2 = new RestyledLabel("label_2", this);
    label_2->show();
    label_2->setAttribute(Qt::WA_DeleteOnClose);

    RestyledLabel *label_3 = new RestyledLabel("label_3", this);
    label_3->show();
    label_3->setAttribute(Qt::WA_DeleteOnClose);

    verticalLayout->addWidget(label_1);
    verticalLayout->addWidget(label_2);
    verticalLayout->addWidget(label_3);

    setAcceptDrops(true);
    setMinimumSize(100, 100);
}

void DragableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(this->isReportDraggingOn && this->draggingReports != NULL){
        this->draggingReports->setText(this->draggingReports->text().append(" DragIn"));
    }
    if (event->mimeData()->hasText()) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
            event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DragableWidget::dropEvent(QDropEvent *event)
{
        QString piece = event->mimeData()->text();
        QPoint position = event->pos();
        QPoint hotSpot;

        QList<QByteArray> hotSpotPos = event->mimeData()->data("application/x-hotspot").split(' ');
        if (hotSpotPos.size() == 2) {
            hotSpot.setX(hotSpotPos.first().toInt());
            hotSpot.setY(hotSpotPos.last().toInt());
        }

        RestyledLabel *droppedLabel = new RestyledLabel(piece, this);
        if(this->layout()){
            this->layout()->addWidget(droppedLabel);
        }
        droppedLabel->setObjectName(droppedLabel->text());
        droppedLabel->move(position - hotSpot);
        droppedLabel->show();
        droppedLabel->setAttribute(Qt::WA_DeleteOnClose);

        position += QPoint(droppedLabel->width(), 0);

        event->setDropAction(Qt::MoveAction);
        event->accept();
        if(this->isReportDraggingOn && this->draggingReports != NULL){
            this->draggingReports->setText(this->draggingReports->text()
                                           .append(" DropIn ").append(droppedLabel->objectName()));
        }
}

void DragableWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPoint hotSpot = event->pos() - child->pos();

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(child->text());
    mimeData->setData("application/x-hotspot",
                      QByteArray::number(hotSpot.x()) + " " + QByteArray::number(hotSpot.y()));

    QPixmap pixmap(child->size());
    child->render(&pixmap);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);

    if (dropAction == Qt::MoveAction)
        child->close();
}

void DragableWidget::setDraggingReportsDestination(QLabel* destination){
    this->isReportDraggingOn = true;
    this->draggingReports = destination;
}
