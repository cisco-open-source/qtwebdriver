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

    QLabel *eventInfo = new QLabel("Nothing happened. ", this);
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
