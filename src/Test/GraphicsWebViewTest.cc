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

#include "GraphicsWebViewTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

GraphicsWebViewTestWindows::GraphicsWebViewTestWindows(QWidget *parent) :
    QGraphicsView(parent) {
    this->setWindowTitle("Test Widget");

    QGraphicsScene *pScene = new QGraphicsScene();
    pWebView = new QGraphicsWebView();
    pScene->addItem(pWebView);

    this->setScene(pScene);
}

GraphicsWebViewTestWindows::~GraphicsWebViewTestWindows() {
    delete pWebView;
}
