#include "qml_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "q_content_type_resolver.h"

#include "extension_qt/widget_view_handle.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtDeclarative/QDeclarativeView>


namespace webdriver {

bool QQmlViewUtil::isUrlSupported(const std::string& url) {
    scoped_ptr<QNetworkAccessManager> pmanager(new QNetworkAccessManager());
    scoped_ptr<QContentTypeResolver> presolver(new QContentTypeResolver(pmanager.get()));
    std::string contentType;
    scoped_ptr<Error> ignore_err(presolver->resolveContentType(url, contentType));

    if (ignore_err != NULL) {
        return false;
    }

    // TODO: add file detection

    return isContentTypeSupported(contentType);
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
