#include "extension_qt/web_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/widget_view_handle.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#else
#include <QtWebKit/QtWebKit>
#include <QtGui/QWidget>
#include <QtGui/QApplication>
#endif

namespace webdriver {

void WebViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
	session->logger().Log(kInfoLogLevel, ">>>>> WebView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        if (pWidget->isHidden()) continue;

        QWebView* pView = qobject_cast<QWebView*>(pWidget);
        if (pView != NULL) {
          	ViewHandle* handle = new QViewHandle(pView);
           	ViewId viewId = session->GetViewForHandle(handle);
            if (!viewId.is_valid()) {
              	if (session->AddNewView(handle, &viewId))  {
           			session->logger().Log(kInfoLogLevel,
                		"WebViewEnumerator found new view("+viewId.id()+")");
                }
            }
            if (viewId.is_valid()) {
                views->insert(viewId);
            }
        }
    }
}

} // namespace webdriver
