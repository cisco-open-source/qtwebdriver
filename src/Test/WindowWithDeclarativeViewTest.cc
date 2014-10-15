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

#include "WindowWithDeclarativeViewTest.h"

WindowWithDeclarativeViewTestWidget::WindowWithDeclarativeViewTestWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Test Window");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(loadQML()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("loadButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(loadQML()));

    pLabel = new QLabel("Status");
    pLabel->setObjectName("labelStatus");
    pLabel->setVisible(true);

    pView = new QDeclarativeView();
    pView->setObjectName("declarativeView");

    connect(pView, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(displayStatus()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);

    QVBoxLayout *vbl = new QVBoxLayout(this);
    vbl->addLayout(hbl);
    vbl->addWidget(pLabel);

    vbl->addWidget(pView);

    this->setLayout(vbl);
}

WindowWithDeclarativeViewTestWidget::~WindowWithDeclarativeViewTestWidget() { }

void WindowWithDeclarativeViewTestWidget::loadQML() {
    pView->setSource(QUrl(pLineEdit->text()));
    pView->show();
}

void WindowWithDeclarativeViewTestWidget::displayStatus() {

    if (pView->status() == QDeclarativeView::Ready) {
        pLabel->setText("Loading successfully");
        return;
    }

    if (pView->status() == QDeclarativeView::Error) {
        pLabel->setText("Error");
    }
}
