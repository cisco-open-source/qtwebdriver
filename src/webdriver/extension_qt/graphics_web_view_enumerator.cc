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
