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
