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

#include "extension_qt/graphics_web_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/graphics_web_view_handle.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QApplication>
#else
#include <QtGui/QGraphicsView>
#include <QtGui/QApplication>
#endif


namespace webdriver {

void GraphicsWebViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    session->logger().Log(kInfoLogLevel, ">>>>> QGraphicsWebView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets()) {
        if (pWidget->isHidden()) continue;

        QGraphicsView* pView = qobject_cast<QGraphicsView*>(pWidget);
        if (NULL != pView) {
            QList<QGraphicsItem*>  childs = pView->items();
            foreach(QGraphicsItem *child, childs) {
                QGraphicsWebView* webView = qobject_cast<QGraphicsWebView*>(child->toGraphicsObject());
                if (webView) {
                    // found
                    ViewHandlePtr handle(new GraphicsWebViewHandle(webView));
                    ViewId viewId = session->GetViewForHandle(handle);
                    if (!viewId.is_valid()) {
                        if (session->AddNewView(handle, &viewId))  {
                            session->logger().Log(kInfoLogLevel,
                                "GraphicsWebViewEnumerator found new view("+viewId.id()+")");
                        }
                    }
                    if (viewId.is_valid()) {
                        views->insert(viewId);
                    }
                }
            }
        }
    }
}

} // namespace webdriver
