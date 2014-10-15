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

#include "WindowWithSeparatedDeclarativeAndWebViewsTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

#include <QtDeclarative/QDeclarativeView>

WindowWithSeparatedDeclarativeAndWebViewsTestWidget::WindowWithSeparatedDeclarativeAndWebViewsTestWidget(QWidget *parent) :
    QWidget(parent) {
    this->setWindowTitle("Test Widget");
    pLineEditForWeb = new QLineEdit();
    pLineEditForWeb->setObjectName("inputWebURL");
    connect(pLineEditForWeb, SIGNAL(returnPressed()), this, SLOT(openWebView()));

    pButtonWeb = new QPushButton("Go to Web!");
    pButtonWeb->setObjectName("openWebViewButton");
    connect(pButtonWeb, SIGNAL(clicked()), this, SLOT(openWebView()));

    pLineEditForQml = new QLineEdit();
    pLineEditForQml->setObjectName("inputQmlURL");
    connect(pLineEditForQml, SIGNAL(returnPressed()), this, SLOT(openQmlView()));

    pButtonQml = new QPushButton("Go to Qml!");
    pButtonQml->setObjectName("openQmlViewButton");
    connect(pButtonQml, SIGNAL(clicked()), this, SLOT(openQmlView()));

    QHBoxLayout* hblweb = new QHBoxLayout();
    hblweb->addWidget(pLineEditForWeb);
    hblweb->addWidget(pButtonWeb);

    QHBoxLayout* hblqml = new QHBoxLayout();
    hblqml->addWidget(pLineEditForQml);
    hblqml->addWidget(pButtonQml);

    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->addLayout(hblweb);
    vbl->addLayout(hblqml);
    this->setLayout(vbl);
}

WindowWithSeparatedDeclarativeAndWebViewsTestWidget::~WindowWithSeparatedDeclarativeAndWebViewsTestWidget() {}

void WindowWithSeparatedDeclarativeAndWebViewsTestWidget::openWebView() {
    QWebView *pWebView = new QWebView;
    pWebView->setAttribute(Qt::WA_DeleteOnClose);
    pWebView->load(pLineEditForWeb->text());
    pWebView->show();
}

void WindowWithSeparatedDeclarativeAndWebViewsTestWidget::openQmlView() {
    QDeclarativeView *pView = new QDeclarativeView();
    pView->setAttribute(Qt::WA_DeleteOnClose);
    pView->setSource(pLineEditForQml->text());
    pView->show();
}
