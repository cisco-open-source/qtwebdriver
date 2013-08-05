#include "qml_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "q_content_type_resolver.h"

#include "extension_qt/widget_view_handle.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtDeclarative/QDeclarativeView>
#include <QtCore/QFileInfo>


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

QDeclarativeView* QQmlViewUtil::getQMLView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qobject_cast<QDeclarativeView*>(qViewHandle->get());
}


} // namespace webdriver
