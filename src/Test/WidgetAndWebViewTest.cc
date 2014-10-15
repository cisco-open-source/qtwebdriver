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

#include "WidgetAndWebViewTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

WidgetAndWebViewTestWindows::WidgetAndWebViewTestWindows(QWidget *parent) :
    QWidget(parent) {
    this->setWindowTitle("Test Widget");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(openWebView()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("openViewButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(openWebView()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);
    this->setLayout(hbl);

}

WidgetAndWebViewTestWindows::~WidgetAndWebViewTestWindows() {}

void WidgetAndWebViewTestWindows::openWebView() {
    QWebView *pWebView = new QWebView;
    pWebView->setAttribute(Qt::WA_DeleteOnClose);
    pWebView->load(pLineEdit->text());
    pWebView->show();

}
