#include "widget_view_util.h"

namespace webdriver {

bool QWidgetViewUtil::isUrlSupported(const std::string& url) {
	const std::string widget_prefix("qtwidget://");

	if (url.compare(0, widget_prefix.length(), widget_prefix) == 0) {
		return true;
	}

	return false;
}	


} // namespace webdriver
