#include "web_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "q_content_type_resolver.h"

#include "extension_qt/widget_view_handle.h"
#include <QtNetwork/QNetworkAccessManager>


namespace webdriver {

bool QWebViewUtil::isUrlSupported(QWebView* pWebView, const std::string& url, Error **error) {
    QWebPage* pWebPage = pWebView->page();
    if (NULL == pWebPage) {
        *error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " Invalid QWebPage* ");
        return false;
    }

    QNetworkAccessManager *pmanager =  pWebPage->networkAccessManager();
    if (NULL == pmanager) {
        *error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " Invalid QNetworkAccessManager* ");
        return false;
    }
    QContentTypeResolver *presolver = new QContentTypeResolver(pmanager);

    std::string mimeType ;
    *error = presolver->resolveContentType(url, mimeType);
    if (NULL != *error) {
        return false;
    }
    return pWebPage->supportsContentType(QString::fromStdString(mimeType));
}	

QWebView* QWebViewUtil::getWebView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qobject_cast<QWebView*>(qViewHandle->get());
}


} // namespace webdriver
