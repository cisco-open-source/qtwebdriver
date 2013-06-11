#include "widget_view_util.h"

#include "webdriver_session.h"

#include "extension_qt/widget_view_handle.h"

namespace webdriver {

const char QWidgetViewUtil::url_protocol[] = "qtwidget://";

bool QWidgetViewUtil::isUrlSupported(const std::string& url) {
	const std::string widget_prefix(url_protocol);

	if (url.compare(0, widget_prefix.length(), widget_prefix) == 0) {
		return true;
	}

	return false;
}	

std::string QWidgetViewUtil::extractClassName(const std::string& url) {
	const std::string widget_prefix(url_protocol);	

	return url.substr(widget_prefix.length());
}

QWidget* QWidgetViewUtil::getView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qViewHandle->get();
}


} // namespace webdriver
