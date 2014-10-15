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

#include "WindowWithEmbeddedViewTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif


WindowWithEmbeddedViewTestWidget::WindowWithEmbeddedViewTestWidget(QWidget *parent) :
    QWidget(parent) {
    setWindowTitle("Test Window");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(loadHTML()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("loadButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(loadHTML()));

    pLabel = new QLabel("Here Will Be HTML Title");
    pLabel->setObjectName("labelTitle");
    pLabel->setVisible(true);

    pView = new QWebView();
    pView->setObjectName("webView");
    connect(pView, SIGNAL(loadFinished(bool)), this, SLOT(setHTMLTitle()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);

    QVBoxLayout *vbl = new QVBoxLayout(this);
    vbl->addLayout(hbl);
    vbl->addWidget(pLabel);
    vbl->addWidget(pView);
    this->setLayout(vbl);
}

WindowWithEmbeddedViewTestWidget::~WindowWithEmbeddedViewTestWidget() {}

void WindowWithEmbeddedViewTestWidget::loadHTML() {
    pView->load(QUrl(pLineEdit->text()));
}

void WindowWithEmbeddedViewTestWidget::setHTMLTitle() {
    pLabel->setText(pView->title());
}
