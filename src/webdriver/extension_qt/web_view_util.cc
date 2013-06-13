#include "web_view_util.h"

#include "webdriver_session.h"

#include "extension_qt/widget_view_handle.h"

namespace webdriver {

bool QWebViewUtil::isUrlSupported(const std::string& url) {
	// TODO: implement content detection
	const std::string non_web_prefix("qtwidget://");

	if (url.compare(0, non_web_prefix.length(), non_web_prefix) != 0) {
		return true;
	}

	return false;
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
