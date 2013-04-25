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
