#include "extension_qt/qml_web_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_qt/declarative_item_view_handle.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#include <QtGui/QWidget>
#include <QtDeclarative/QDeclarativeView>
#include <QtGui/QApplication>

namespace webdriver {

void QmlWebViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    session->logger().Log(kInfoLogLevel, ">>>>> QDeclarativeWebView enumerate");

    foreach(QWidget* pWidget, qApp->allWidgets()) {
        if (pWidget->isHidden()) continue;

        QDeclarativeView* pView = qobject_cast<QDeclarativeView*>(pWidget);
        if (NULL != pView) {

            QDeclarativeItem *parentItem = qobject_cast<QDeclarativeItem*>(pView->rootObject());

            if (NULL == parentItem) continue;

            QList<QDeclarativeItem*> childs = parentItem->findChildren<QDeclarativeItem*>();
            childs.append(parentItem);
            foreach(QDeclarativeItem *child, childs) {
                if (isWebView(child)) {
                    // found
                    ViewHandlePtr handle(new QDeclarativeItemViewHandle(child));
                    ViewId viewId = session->GetViewForHandle(handle);
                    if (!viewId.is_valid()) {
                        if (session->AddNewView(handle, &viewId))  {
                            session->logger().Log(kInfoLogLevel,
                                "QmlWebViewEnumerator found new view("+viewId.id()+")");
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

bool QmlWebViewEnumeratorImpl::isWebView(QDeclarativeItem* item) const {
    if (NULL == item) return false;
    if (!item->isVisible()) return false;
    if (!item->isEnabled()) return false;

    // check if element is QML WebView
    QString className(item->metaObject()->className());
    className.remove(QRegExp(QLatin1String("_QMLTYPE_\\d+"))); \
    className.remove(QRegExp(QLatin1String("_QML_\\d+"))); \
    if (className.startsWith(QLatin1String("QDeclarative"))) className = className.mid(12);

    if (className == "WebView") {
        return true;
    }

    return false;
}

} // namespace webdriver
