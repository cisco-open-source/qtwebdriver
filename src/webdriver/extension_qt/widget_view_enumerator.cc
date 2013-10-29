#include "extension_qt/widget_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/widget_view_handle.h"


#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#else
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#endif

namespace webdriver {

void WidgetViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
	session->logger().Log(kInfoLogLevel, ">>>>> WidgetView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        if (pWidget->isHidden()) continue;

        if (!pWidget->isWindow()) continue;

        if(NULL != qobject_cast<QMessageBox*>(pWidget)) continue; //check if it's an alert

        ViewHandlePtr handle(new QViewHandle(pWidget));
        ViewId viewId = session->GetViewForHandle(handle);
        if (!viewId.is_valid()) {
            if (session->AddNewView(handle, &viewId))  {
           		session->logger().Log(kInfoLogLevel,
                	"WidgetViewEnumerator found new view("+viewId.id()+")");
            }
        }
        if (viewId.is_valid()) {
            views->insert(viewId);
        }
    }
}

} // namespace webdriver
