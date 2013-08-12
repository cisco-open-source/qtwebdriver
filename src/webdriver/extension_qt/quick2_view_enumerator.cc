#include "extension_qt/quick2_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/quick2_view_handle.h"

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

        QQuickWindow* pView = qobject_cast<QQuickWindow*>(pWindow);
        if (pView != NULL) {
            ViewHandlePtr handle(new QQuick2ViewHandle(pView));
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
