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

#include "extension_qt/quick2_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/qwindow_view_handle.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>

namespace webdriver {

void Quick2ViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    session->logger().Log(kInfoLogLevel, ">>>>> Quick2View enumerate");

    foreach(QWindow* pWindow, QGuiApplication::allWindows())
    {
        if (!pWindow->isVisible()) continue;

        QQuickView* pView = qobject_cast<QQuickView*>(pWindow);
        if (pView != NULL) {
            ViewHandlePtr handle(new QWindowViewHandle(pView));
            ViewId viewId = session->GetViewForHandle(handle);
            if (!viewId.is_valid()) {
                if (session->AddNewView(handle, &viewId))  {
                    session->logger().Log(kInfoLogLevel,
                        "Quick2ViewEnumerator found new view("+viewId.id()+")");
                }
            }
            if (viewId.is_valid()) {
                views->insert(viewId);
            }
        }
    }
}

} // namespace webdriver
