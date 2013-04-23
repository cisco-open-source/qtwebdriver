/* FindingTest.cc
  */

#include "FindingTest.h"

static QString TEXT_EDIT_NAME="textEdit";
static QString SCROLL_AREA_NAME="scrollArea";
static QString HIDDEN_LABEL_NAME="hiddenLabel";

FindingTestWidget::FindingTestWidget()
    : textEdit(new QTextEdit()),
      scrollArea(new QScrollArea),
      hiddenLabel(new QLabel)
{
    textEdit->setObjectName(TEXT_EDIT_NAME);
    scrollArea->setObjectName(SCROLL_AREA_NAME);

    QRect labelGeometry(scrollArea->geometry());
    labelGeometry.setTopLeft(QPoint(30, 30));
    hiddenLabel->setGeometry(labelGeometry);
    scrollArea->setWidget(hiddenLabel);
}

FindingTestWidget::~FindingTestWidget()
{
    delete textEdit;
    delete scrollArea;
    delete hiddenLabel;
}
