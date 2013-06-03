#include "extension_qt/web_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include <QtGui/QApplication>
#include <QtWebKit/QtWebKit>
#include <QtGui/QWidget>

namespace webdriver {

void WebViewEnumeratorImpl::EnumerateViews(Session* session) const {
	session->logger().Log(kInfoLogLevel, ">>>>> WebView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        if (pWidget->isHidden()) continue;

        QWebView* pView = qobject_cast<QWebView*>(pWidget);
        if (pView != NULL) {
          	ViewHandle handle = static_cast<ViewHandle>(pView);
           	ViewId viewId = session->GetViewForHandle(handle);
            if (!viewId.is_valid()) {
              	if (session->AddNewView(handle, &viewId))  {
           			session->logger().Log(kInfoLogLevel,
                		"WebViewEnumerator found new view("+viewId.id()+")");
                }
            }
        }
    }
}

} // namespace webdriver