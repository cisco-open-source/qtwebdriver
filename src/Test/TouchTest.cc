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

#include "TouchTest.h"

TouchTestWidget::TouchTestWidget(QWidget *parent) :
    QWidget(parent)
{
    resize(400, 620);

    touchPanel = new QFrame(this);
    touchPanel->installEventFilter(this);
    touchPanel->setObjectName("touchArea");
    touchPanel->setFrameShape(QFrame::Box);
    touchPanel->setAttribute(Qt::WA_AcceptTouchEvents);

    resultLabel = new QLabel(this);
    resultLabel->setObjectName("resultLabel");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(resultLabel);
    layout->addWidget(touchPanel);
    setLayout(layout);

    pressLabel = new QLabel(touchPanel);
    pressLabel->setText("Pressed");
    pressLabel->setObjectName("pressLabel");
    pressLabel->setVisible(false);

    releaseLabel = new QLabel(touchPanel);
    releaseLabel->setText("Released");
    releaseLabel->setObjectName("releaseLabel");
    releaseLabel->setVisible(false);

    moveLabel = new QLabel(touchPanel);
    moveLabel->setText("Move");
    moveLabel->setObjectName("moveLabel");
    moveLabel->setVisible(false);
}


bool TouchTestWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == touchPanel)
    {
        if ((e->type() == QEvent::TouchBegin))
        {
            resultLabel->setText(resultLabel->text()+"Pressed");

            QTouchEvent* te = static_cast<QTouchEvent*>(e);
            if (te)
            {
                pressLabel->setVisible(true);
                QPoint tpos = QPoint(te->touchPoints().at(0).pos().x(), te->touchPoints().at(0).pos().y());
                pressLabel->move(tpos);
            }

        }
        if (e->type() == QEvent::TouchUpdate)
        {
            resultLabel->setText(resultLabel->text()+"Moved");

            QTouchEvent* te = static_cast<QTouchEvent*>(e);
            if (te)
            {
                moveLabel->setVisible(true);
                QPoint tpos = QPoint(te->touchPoints().at(0).pos().x(), te->touchPoints().at(0).pos().y());
                moveLabel->move(tpos);
                moveLabel->resize(100, 20);
                moveLabel->raise();

            }
        }
        if  (e->type() == QEvent::TouchEnd)
        {
            resultLabel->setText(resultLabel->text()+"Released");

            QTouchEvent* te = static_cast<QTouchEvent*>(e);
            if (te)
            {
                releaseLabel->setVisible(true);
                QPoint tpos = QPoint(te->touchPoints().at(0).pos().x(), te->touchPoints().at(0).pos().y());
                releaseLabel->move(tpos);
                qDebug()<<releaseLabel->geometry();
            }
        }
    }
    return false;
}
