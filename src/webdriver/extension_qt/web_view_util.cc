#include "web_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "q_content_type_resolver.h"

#include "extension_qt/widget_view_handle.h"
#include <QtNetwork/QNetworkAccessManager>


namespace webdriver {

bool QWebViewUtil::isUrlSupported(QWebPage* pWebPage, const std::string& url, Error **error) {
    if (NULL == pWebPage) {
        //*error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " Invalid QWebPage* ");
        return false;
    }

    QNetworkAccessManager *pmanager = pWebPage->networkAccessManager();
    if (NULL == pmanager) {
        //*error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " Invalid QNetworkAccessManager* ");
        return false;
    }

    scoped_ptr<QContentTypeResolver> presolver(new QContentTypeResolver(pmanager));

    std::string mimeType;

    *error = presolver->resolveContentType(url, mimeType);
    if (NULL != *error) {
        return false;
    }

    return pWebPage->supportsContentType(QString::fromStdString(mimeType));
}	

bool QWebViewUtil::isUrlSupported(const std::string& url, Error **error) {
    // support blank page
    const std::string BLANK = "about:blank";
    if (!url.compare(BLANK))
        return true;

    scoped_ptr<QNetworkAccessManager> pmanager(new QNetworkAccessManager());
    scoped_ptr<QContentTypeResolver> presolver(new QContentTypeResolver(pmanager.get()));

    std::string mimeType ;
    *error = presolver->resolveContentType(url, mimeType);
    if (NULL != *error) {
        return false;
    }

    scoped_ptr<QWebPage> pWebPage(new QWebPage());

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
