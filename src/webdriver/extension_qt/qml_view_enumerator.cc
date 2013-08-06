#include "extension_qt/qml_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/widget_view_handle.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>

namespace webdriver {

void QmlViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    session->logger().Log(kInfoLogLevel, ">>>>> QmlView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        if (pWidget->isHidden()) continue;

        QDeclarativeView* pView = qobject_cast<QDeclarativeView*>(pWidget);
        if (pView != NULL) {
            ViewHandlePtr handle(new QViewHandle(pView));
            ViewId viewId = session->GetViewForHandle(handle);
            if (!viewId.is_valid()) {
                if (session->AddNewView(handle, &viewId))  {
                    session->logger().Log(kInfoLogLevel,
                        "QmlViewEnumerator found new view("+viewId.id()+")");
                }
            }
            if (viewId.is_valid()) {
                views->insert(viewId);
            }
        }
    }
}

} // namespace webdriver
