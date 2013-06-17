#ifndef DRAGABLEWIDGET_H
#define DRAGABLEWIDGET_H

#include "CommonQtTestHeaders.h"
#include "RestyledLabel.h"

class DragableWidget : public QWidget
{
public:
    DragableWidget(QWidget *parent = 0);
    void setDraggingReportsDestination(QLabel* destination);

protected:
    bool isReportDraggingOn;
    QLabel* draggingReports;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // DRAGABLEWIDGET_H
