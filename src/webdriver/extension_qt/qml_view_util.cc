#include "qml_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "q_content_type_resolver.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QFileInfo>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include "extension_qt/quick2_view_handle.h"
#include <QtQuick/QQuickWindow>
#else
#include "extension_qt/widget_view_handle.h"
#include <QtDeclarative/QDeclarativeView>
#endif


namespace webdriver {

bool QQmlViewUtil::isUrlSupported(const std::string& url) {
    QUrl qUrl(url.c_str());
    QString scheme = qUrl.scheme();

    if ( (0 == scheme.compare("http", Qt::CaseInsensitive)) ||
         (0 == scheme.compare("https", Qt::CaseInsensitive)) ) {

        scoped_ptr<QNetworkAccessManager> pmanager(new QNetworkAccessManager());
        scoped_ptr<QContentTypeResolver> presolver(new QContentTypeResolver(pmanager.get()));
        std::string contentType;

        scoped_ptr<Error> ignore_err(presolver->resolveContentType(url, contentType));

        if (ignore_err != NULL) {
            return false;
        }

        return isContentTypeSupported(contentType);
    } else {
        // detection fy filename
        QFileInfo fileInfo(qUrl.path());
        if (0 == fileInfo.suffix().compare("qml", Qt::CaseInsensitive)) {
            return true;
        }
    }


    return false;
}

bool QQmlViewUtil::isContentTypeSupported(const std::string& mime) {
    if (mime == "application/qml") 
        return true;

    return false;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QQuickWindow* QQmlViewUtil::getQMLView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    Quick2ViewHandle* qViewHandle = dynamic_cast<Quick2ViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QQuickWindow*>(qViewHandle->get());
}
#else
QDeclarativeView* QQmlViewUtil::getQMLView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qobject_cast<QDeclarativeView*>(qViewHandle->get());
}
#endif


} // namespace webdriver
